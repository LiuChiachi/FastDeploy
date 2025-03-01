// Copyright (c) 2022 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <type_traits>

#include "fastdeploy/fastdeploy_runtime.h"

#ifdef ENABLE_VISION
#include "fastdeploy/vision.h"
#endif

namespace fastdeploy {

void BindBackend(pybind11::module&);
void BindVision(pybind11::module&);

pybind11::dtype FDDataTypeToNumpyDataType(const FDDataType& fd_dtype);

FDDataType NumpyDataTypeToFDDataType(const pybind11::dtype& np_dtype);

void PyArrayToTensor(pybind11::array& pyarray, FDTensor* tensor,
                     bool share_buffer = false);
pybind11::array TensorToPyArray(const FDTensor& tensor);

#ifdef ENABLE_VISION
cv::Mat PyArrayToCvMat(pybind11::array& pyarray);
#endif

template <typename T>
FDDataType CTypeToFDDataType() {
  if (std::is_same<T, int32_t>::value) {
    return FDDataType::INT32;
  } else if (std::is_same<T, int64_t>::value) {
    return FDDataType::INT64;
  } else if (std::is_same<T, float>::value) {
    return FDDataType::FP32;
  } else if (std::is_same<T, double>::value) {
    return FDDataType::FP64;
  }
  FDASSERT(false,
           "CTypeToFDDataType only support int32/int64/float32/float64 now.");
  return FDDataType::FP32;
}

template <typename T>
std::vector<pybind11::array> PyBackendInfer(
    T& self, const std::vector<std::string>& names,
    std::vector<pybind11::array>& data) {
  std::vector<FDTensor> inputs(data.size());
  for (size_t i = 0; i < data.size(); ++i) {
    // TODO(jiangjiajun) here is considered to use user memory directly
    auto dtype = NumpyDataTypeToFDDataType(data[i].dtype());
    std::vector<int64_t> data_shape;
    data_shape.insert(data_shape.begin(), data[i].shape(),
                      data[i].shape() + data[i].ndim());
    inputs[i].Resize(data_shape, dtype);
    memcpy(inputs[i].MutableData(), data[i].mutable_data(), data[i].nbytes());
    inputs[i].name = names[i];
  }

  std::vector<FDTensor> outputs(self.NumOutputs());
  self.Infer(inputs, &outputs);

  std::vector<pybind11::array> results;
  results.reserve(outputs.size());
  for (size_t i = 0; i < outputs.size(); ++i) {
    auto numpy_dtype = FDDataTypeToNumpyDataType(outputs[i].dtype);
    results.emplace_back(pybind11::array(numpy_dtype, outputs[i].shape));
    memcpy(results[i].mutable_data(), outputs[i].Data(),
           outputs[i].Numel() * FDDataTypeSize(outputs[i].dtype));
  }
  return results;
}

}  // namespace fastdeploy
