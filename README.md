# Perception

计算机视觉与感知算法库，包含图像拼接、AprilTag 检测等模块。

## 模块列表

### [AprilTag](./thirdparty/apriltag/)

第三方库 [AprilRobotics/apriltag](https://github.com/AprilRobotics/apriltag) 官方源码。编译与 API 说明见 [thirdparty/apriltag/README.md](./thirdparty/apriltag/README.md)。鱼眼标定中的用法见 [fisheye_image_stitching/README.md](./image_stitching/fisheye_image_stitching/README.md)。

```bash
cd thirdparty/apriltag
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### [图像拼接 (Image Stitching)](./image_stitching/)

多相机图像拼接解决方案，支持鱼眼和平面相机：

- **[鱼眼图像拼接](./image_stitching/fisheye_image_stitching/)** — 基于四路鱼眼相机的车辆环视（AVM/Surround View）全景拼接，支持 3D 碗面投影和 2D 鸟瞰展开视图
- **[平面图像拼接](./image_stitching/planar_image_stitching/)** — 基于双目平面相机的宽幅全景拼接，适用于前向监控、全景摄像等场景

两个模块均针对 SpacemiT RISC-V 平台优化，使用 MPP 硬件解码与 GPU 实时渲染。

## 平台支持

- **硬件平台**：SpacemiT X100 (RISC-V 8 核) + PowerVR B-Series BXM-4-64 GPU
- **操作系统**：Bianbu Linux (基于 Debian)
- **图形 API**：OpenGL ES 3.2 (Mesa 24.2)
- **视频解码**：MPP (Media Process Platform) 硬件加速

## 快速开始

每个模块都是独立的，按需选择：

```bash
# AprilTag
cd thirdparty/apriltag && cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build

# 鱼眼拼接（四路相机环视）
cd image_stitching/fisheye_image_stitching
./run_live_vi.sh

# 平面拼接（双目宽幅全景）
cd image_stitching/planar_image_stitching
./run.sh
```

详细的编译、配置和使用说明见各模块的 README.md。

## 目录结构

```
perception/
├── README.md                          # 本文档
├── thirdparty/
│   └── apriltag/                      # AprilTag 官方源码
│       └── README.md                  # 官方文档
└── image_stitching/                   # 图像拼接模块
    ├── README.md                      # 拼接模块总览
    ├── fisheye_image_stitching/       # 鱼眼环视拼接
    │   └── README.md
    └── planar_image_stitching/        # 平面全景拼接
        └── README.md
```

## 第三方依赖

本仓库以下目录包含第三方开源代码，使用时请遵守对应许可证：

| 目录 | 上游项目 | 版本 | 许可证 |
|------|----------|------|--------|
| `thirdparty/apriltag/` | [AprilRobotics/apriltag](https://github.com/AprilRobotics/apriltag) | v3.4.5+ | [BSD 2-Clause](./thirdparty/apriltag/LICENSE.md) |

`thirdparty/apriltag/` 为官方源码 vendoring，核心库未修改。版权归 The Regents of The University of Michigan（2013–2016），详见 [thirdparty/apriltag/LICENSE.md](./thirdparty/apriltag/LICENSE.md)。
