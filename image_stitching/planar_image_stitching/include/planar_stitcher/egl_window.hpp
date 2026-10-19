#ifndef EGL_WINDOW_HPP
#define EGL_WINDOW_HPP
#pragma once

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <cstdint>
#include <string>

namespace planar_stitcher {

struct Nv12DmaFrame;

class EglWindow {
public:
    EglWindow() = default;
    ~EglWindow();
    EglWindow(const EglWindow&) = delete;
    EglWindow& operator = (const EglWindow&) = delete;

    // Opens an on-screen window; falls back to an offscreen pbuffer when no
    // display is available. force_offscreen skips the display attempt entirely.
    bool Open(int width, int height, const char* title, bool force_offscreen,
                std::string* error);
    bool Open(int width, int height, const char* title, std::string* error) {
        return Open(width, height, title, false, error);
    }
    void Close() noexcept;
    bool PollExitRequested();
    bool HasDmaBufImport() const;
    // true when rendering to an offscreen pbuffer instead of a window.
    bool is_offscreen() const { return offscreen_; }
    int width() const { return width_; }
    int height() const { return height_; }
    EGLDisplay display() const { return display_; }
    EGLSurface surface() const { return surface_; }
    PFNEGLCREATEIMAGEKHRPROC create_image() const { return create_image_; }
    PFNEGLDESTROYIMAGEKHRPROC destroy_image() const { return destroy_image_; }
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC image_target_texture() const {
        return image_target_texture_;
    }

private:
    // config_handle is an EGLConfig; kept as void* so the header needs no cast.
    bool FinishOffscreen(void* config_handle, int width, int height,
                        std::string* error);
    bool ResolveDmaBufExtensions(std::string* error);

    void* x_display_ = nullptr;
    unsigned long x_window_ = 0;
    unsigned long x_colormap_ = 0;
    unsigned long wm_delete_window_ = 0;
    bool exit_requested_ = false;
    bool offscreen_ = false;
    int width_ = 0;
    int height_ = 0;
    int gbm_fd_ = -1;
    void* gbm_device_ = nullptr;
    EGLDisplay display_ = EGL_NO_DISPLAY;
    EGLContext context_ = EGL_NO_CONTEXT;
    EGLSurface surface_ = EGL_NO_SURFACE;
    PFNEGLCREATEIMAGEKHRPROC create_image_ = nullptr;
    PFNEGLDESTROYIMAGEKHRPROC destroy_image_ = nullptr;
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC image_target_texture_ = nullptr;
};

class ImportedNv12Texture {
public:
    ImportedNv12Texture() = default;
    ~ImportedNv12Texture();
    ImportedNv12Texture(const ImportedNv12Texture&) = delete;
    ImportedNv12Texture& operator = (const ImportedNv12Texture&) = delete;

    // Chooses the hardware (DMA-BUF) or software (glTexImage2D) path from the
    // frame itself; software frames carry their planes in cpu_y/cpu_uv.
    bool Import(EglWindow& window, const Nv12DmaFrame& frame,
                std::string* error);
    void Reset() noexcept;
    // Valid on the hardware path: the external (samplerExternalOES) texture.
    GLuint texture_id() const { return texture_; }
    // Valid on the software path: separate Y and interleaved-UV textures.
    GLuint y_texture() const { return y_texture_; }
    GLuint uv_texture() const { return uv_texture_; }
    bool is_cpu() const { return cpu_; }

private:
    bool ImportHardware(EglWindow& window, const Nv12DmaFrame& frame,
                        std::string* error);
    bool ImportCpu(const Nv12DmaFrame& frame, std::string* error);

    EglWindow* window_ = nullptr;
    EGLImageKHR image_ = EGL_NO_IMAGE_KHR;
    GLuint texture_ = 0;
    GLuint y_texture_ = 0;
    GLuint uv_texture_ = 0;
    bool cpu_ = false;
};

}  // namespace planar_stitcher

#endif  // EGL_WINDOW_HPP
