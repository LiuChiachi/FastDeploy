import fastdeploy as fd
import cv2
import os


def parse_arguments():
    import argparse
    import ast
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--model", required=True, help="Path of PaddleSeg model.")
    parser.add_argument(
        "--image", type=str, required=True, help="Path of test image file.")
    parser.add_argument(
        "--bg",
        type=str,
        required=True,
        default=None,
        help="Path of test background image file.")
    parser.add_argument(
        "--device",
        type=str,
        default='cpu',
        help="Type of inference device, support 'cpu' or 'gpu'.")
    parser.add_argument(
        "--use_trt",
        type=ast.literal_eval,
        default=False,
        help="Wether to use tensorrt.")
    return parser.parse_args()


def build_option(args):
    option = fd.RuntimeOption()
    if args.device.lower() == "gpu":
        option.use_gpu()
        option.use_paddle_backend()

    if args.use_trt:
        option.use_trt_backend()
        option.set_trt_input_shape("img", [1, 3, 512, 512])

    return option


args = parse_arguments()

# 配置runtime，加载模型
runtime_option = build_option(args)
model_file = os.path.join(args.model, "model.pdmodel")
params_file = os.path.join(args.model, "model.pdiparams")
config_file = os.path.join(args.model, "deploy.yaml")
model = fd.vision.matting.PPMatting(
    model_file, params_file, config_file, runtime_option=runtime_option)

# 预测图片抠图结果
im = cv2.imread(args.image)
bg = cv2.imread(args.bg)
result = model.predict(im.copy())
print(result)
# 可视化结果
vis_im = fd.vision.vis_matting_alpha(im, result)
vis_im_with_bg = fd.vision.swap_background_matting(im, bg, result)
cv2.imwrite("visualized_result_fg.jpg", vis_im)
cv2.imwrite("visualized_result_replaced_bg.jpg", vis_im_with_bg)
print(
    "Visualized result save in ./visualized_result_replaced_bg.jpg and ./visualized_result_fg.jpg"
)
