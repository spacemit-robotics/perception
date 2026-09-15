# Stereo Matching

SpacemiT K3 LAS2 双目视差推理库。

## 目录

```text
stereo_matching/
├── include/stereo_matching.h
├── lib/libstereo_matching.so
├── model/LAS2_M_288x384.fp16.onnx
├── demo/
│   └── demo.cpp
├── CMakeLists.txt
├── README.md
└── run.sh
```

## 运行 demo

左右图作为外部输入：

```bash
STEREO_LEFT=/path/Motorcycle-perfect/im0.png \
STEREO_RIGHT=/path/Motorcycle-perfect/im1.png \
./run.sh
```

默认使用 `model/LAS2_M_288x384.fp16.onnx`，输入宽×高为
`384×288`。demo 会直接 resize 左右图、转换为 RGB，并在模型外按
ImageNet 标准执行 `(RGB / 255 - mean) / std`，其中
`mean=(0.485, 0.456, 0.406)`、`std=(0.229, 0.224, 0.225)`。
默认配置是 8 个 A100 核和 4 个实例。可通过以下环境变量
调整：

- `STEREO_MODEL`
- `STEREO_A100_CORES`
- `STEREO_INSTANCES`
- `STEREO_OUTPUT`

## API

```cpp
#include <stereo_matching.h>

spacemit::stereo::Config config;
config.model_path = "./model/LAS2_M_288x384.fp16.onnx";
config.a100_core_count = 8;
config.instance_count = 8;

spacemit::stereo::StereoMatcher matcher;
if (!matcher.initialize(config)) {
    // matcher.last_error()
}
```

客户程序只需增加：

```text
-I<stereo_matching>/include
-L<stereo_matching>/lib -lstereo_matching
```

`model_info()` 返回模型输入尺寸和布局。`infer()` 接收 ImageNet
归一化后的 RGB float32 输入。输入输出内存在 `infer()` 返回前必须保持
有效。

## 板端验证

当前包使用 K3 yyx EP 和 `LAS2_M_288x384.fp16.onnx`。在 K3 板端以
Middlebury Motorcycle 左右图完成了功能验证；demo 打印单次端到端推理
调用耗时。性能取决于 A100 核数、实例数和输入并发，应在目标部署配置下
预热后测量。

OpenCV 仅用于 `demo/demo.cpp` 读取和保存图片。
