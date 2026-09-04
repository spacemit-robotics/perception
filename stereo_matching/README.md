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
`384×288`。默认配置是 8 个 A100 核和 4 个实例。可通过以下环境变量
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

`model_info()` 返回模型输入尺寸和布局。`infer()` 接收 RGB float32
输入，数值范围为 `0–255`。输入输出内存在 `infer()` 返回前必须保持
有效。

## 板端实测

K3，`384×288`，预热 10 次，纯推理：

| 配置 | 并发 | 平均延迟 | 吞吐 |
|---|---:|---:|---:|
| 4 核 / 1 实例 | 1 | 50.04 ms | 19.98 FPS |
| 8 核 / 4 实例 | 4 | 92.44 ms | 43.11 FPS |
| 8 核 / 8 实例 | 8 | 160.03 ms | 49.70 FPS |

8 核 / 8 实例连续 800 次：P50 `159.87 ms`，P99 `165.17 ms`。

OpenCV 仅用于 `demo/demo.cpp` 读取和保存图片。
