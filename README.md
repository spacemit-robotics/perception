# Perception

计算机视觉与感知算法库，包含图像拼接等模块。

## 模块列表

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
└── image_stitching/                   # 图像拼接模块
    ├── README.md                      # 拼接模块总览
    ├── fisheye_image_stitching/       # 鱼眼环视拼接
    │   └── README.md
    └── planar_image_stitching/        # 平面全景拼接
        └── README.md
```
