#ifndef PLANAR_RENDERER_HPP
#define PLANAR_RENDERER_HPP
#pragma once

#include "planar_stitcher/blend_pipeline.hpp"
#include "planar_stitcher/egl_window.hpp"
#include "planar_stitcher/feature_matcher.hpp"
#include "planar_stitcher/mpp_jpeg_decoder.hpp"

#include <string>
#include <vector>

namespace planar_stitcher {

class PlanarRenderer {
public:
    static constexpr int kOutputWidth = 1920;
    static constexpr int kOutputHeight = 1080;
    ~PlanarRenderer();
    bool Initialize(EglWindow& window, std::string* error);
    bool Prepare(const RegistrationResult& registration,
                const BlendMaskPyramid& masks, std::string* error);
    bool RenderFrame(const Nv12DmaFrame& left, const Nv12DmaFrame& right,
                    const ExposureModel& exposure, std::string* error);
    bool Readback(std::vector<uint8_t>* bgr, std::string* error);
    bool Present(std::string* error);
    int width() const { return width_; }
    int height() const { return height_; }
    int output_width() const { return kOutputWidth; }
    int output_height() const { return kOutputHeight; }
    int bands() const { return static_cast<int>(levels_.size()); }

private:
    struct LevelResources {
        int width = 0;
        int height = 0;
        GLuint left = 0;
        GLuint right = 0;
        GLuint mask = 0;
        GLuint reconstruction = 0;
        GLuint left_blur = 0;
        GLuint right_blur = 0;
    };

    void ReleaseResources() noexcept;

    EglWindow* window_ = nullptr;
    RegistrationResult registration_;
    std::vector<LevelResources> levels_;
    GLuint projection_program_ = 0;
    GLuint projection_cpu_program_ = 0;
    GLuint horizontal_program_ = 0;
    GLuint downsample_program_ = 0;
    GLuint coarse_program_ = 0;
    GLuint reconstruct_program_ = 0;
    GLuint final_program_ = 0;
    GLuint preview_program_ = 0;
    GLuint fbo_ = 0;
    GLuint color_ = 0;
    int width_ = 0;
    int height_ = 0;
};

std::vector<uint8_t> ConvertBottomUpRgbaToBgr(const uint8_t* rgba,
                                                int width, int height);

}  // namespace planar_stitcher

#endif  // PLANAR_RENDERER_HPP
