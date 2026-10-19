#ifndef MPP_JPEG_DECODER_HPP
#define MPP_JPEG_DECODER_HPP
#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace planar_stitcher {

// A decoded NV12 frame.
//
// Hardware path (VDEC): the pixels live in a DMA-BUF, identified by dma_fd plus
// the plane strides/offset, and are referenced through y_data/uv_data. This
// allows a zero-copy EGL import but needs access to /dev/video*, which is only
// granted to a session attached to the active seat (a desktop login).
//
// Software path (CPU JPEG decode): the pixels live in owned storage
// (cpu_y/cpu_uv) and dma_fd stays -1. The renderer then uploads via glTexImage2D
// instead of importing a DMA-BUF. y_data/uv_data point into that storage so
// callers that only read luma (registration, blending) work unchanged.
struct Nv12DmaFrame {
    int width = 0;
    int height = 0;
    int dma_fd = -1;
    int channel = -1;
    std::uint32_t y_stride = 0;
    std::uint32_t uv_stride = 0;
    std::uint32_t uv_offset = 0;
    const std::uint8_t* y_data = nullptr;
    const std::uint8_t* uv_data = nullptr;
    unsigned long buffer_id = 0;
    // Software path storage. Empty on the hardware path.
    std::vector<std::uint8_t> cpu_y;
    std::vector<std::uint8_t> cpu_uv;

    bool is_hardware() const { return dma_fd >= 0; }
};

class MppJpegDecoder {
public:
    MppJpegDecoder() = default;
    ~MppJpegDecoder();

    MppJpegDecoder(const MppJpegDecoder&) = delete;
    MppJpegDecoder& operator = (const MppJpegDecoder&) = delete;

    bool Open(std::string* error);
    bool DecodeFile(const std::string& path, Nv12DmaFrame* frame,
                    std::string* error);
    void Release(Nv12DmaFrame* frame) noexcept;
    void Close() noexcept;

private:
    static constexpr int kMaxChannels = 64;
    struct ChannelState {
        bool created = false;
        bool enabled = false;
        unsigned long buffer_id = 0;
    };

    void DestroyChannel(int channel) noexcept;
    int AllocateChannel() noexcept;
    // Decode a JPEG to NV12 entirely on the CPU. Used when VDEC cannot open its
    // device node, which is the case whenever the process has no desktop login.
    bool DecodeOnCpu(const std::vector<std::uint8_t>& jpeg, std::uint32_t width,
        std::uint32_t height, const std::string& path,
        Nv12DmaFrame* frame, std::string* error);

    int next_channel_ = 0;
    std::array<ChannelState, kMaxChannels> channels_{};
    bool vdec_initialized_ = false;
    bool vb_initialized_ = false;
    bool sys_initialized_ = false;
    // Set once VDEC proves unusable so subsequent decodes skip the hardware path.
    bool hardware_unavailable_ = false;
};

}  // namespace planar_stitcher

#endif  // MPP_JPEG_DECODER_HPP
