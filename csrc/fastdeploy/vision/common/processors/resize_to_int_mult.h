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

#include "fastdeploy/vision/common/processors/base.h"

namespace fastdeploy {
namespace vision {

class ResizeToIntMult : public Processor {
 public:
  explicit ResizeToIntMult(int mult_int = 32, int interp = 1) {
    mult_int_ = mult_int;
    interp_ = interp;
  }
  bool CpuRun(Mat* mat);
#ifdef ENABLE_OPENCV_CUDA
  bool GpuRun(Mat* mat);
#endif
  std::string Name() { return "ResizeToIntMult"; }

  static bool Run(Mat* mat, int mult_int = 32, int interp = 1,
                  ProcLib lib = ProcLib::OPENCV_CPU);

 private:
  int interp_;
  int mult_int_;
};
}  // namespace vision
}  // namespace fastdeploy
