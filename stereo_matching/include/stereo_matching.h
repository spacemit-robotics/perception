#ifndef SPACEMIT_STEREO_MATCHING_H
#define SPACEMIT_STEREO_MATCHING_H

#include <cstddef>

#if defined(_WIN32)
#define STEREO_MATCHING_API __declspec(dllimport)
#elif defined(__GNUC__)
#define STEREO_MATCHING_API __attribute__((visibility("default")))
#else
#define STEREO_MATCHING_API
#endif

namespace spacemit::stereo {

enum class TensorLayout {
    nchw,
    nhwc,
};

struct Config {
    // The ONNX model is loaded while initialize() creates the facade workers
    // and their A100 EP sessions.
    const char* model_path = nullptr;

    // A100 CPUs are allocated consecutively from first_a100_cpu. Cores are
    // balanced over instance_count EP instances automatically.
    int a100_core_count = 8;
    int instance_count = 4;
    int first_a100_cpu = 8;
    int first_x100_cpu = 0;

    // Initialization clears stale TCM ownership before any EP session exists.
    bool clear_tcm = true;
    const char* tcm_clear_program = "/usr/bin/spacemit-tcm-smi";

    // Keep facade workers on X100 by default. SpaceMIT EP creates the actual
    // compute threads on the configured A100 CPUs; switching the facade worker
    // too would make two compute threads contend for each A100 core.
    // Enable this only for legacy diagnostics, not normal multi-instance use.
    bool switch_workers_to_a100 = false;
    // Used only when switch_workers_to_a100 is enabled. Normal operation lets
    // SpaceMIT EP switch and bind its own A100 compute threads internally.
    const char* ai_thread_control = "/proc/set_ai_thread";
};

struct ModelInfo {
    int input_width = 0;
    int input_height = 0;
    std::size_t input_elements = 0;
    int output_width = 0;
    int output_height = 0;
    std::size_t output_elements = 0;
    TensorLayout input_layout = TensorLayout::nchw;
};

class STEREO_MATCHING_API StereoMatcher {
public:
    StereoMatcher();
    ~StereoMatcher();

    StereoMatcher(const StereoMatcher&) = delete;
    StereoMatcher& operator=(const StereoMatcher&) = delete;

    // Loads the model and creates the configured EP instances. Returns only
    // after all worker-owned sessions are ready.
    bool initialize(const Config& config);

    // Synchronous zero-copy handoff. left and right are float32 RGB tensors in
    // the layout and capacity reported by model_info(); pixel values use the
    // model's LAS2 convention (currently 0..255). disparity is float32.
    // All three buffers are wrapped by ONNX Runtime directly and must remain
    // valid until this call returns.
    // Concurrent calls are distributed over available instances and block on
    // semaphores; workers never poll or spin.
    bool infer(const float* left,
               std::size_t left_elements,
               const float* right,
               std::size_t right_elements,
               float* disparity,
               std::size_t disparity_elements,
               int timeout_ms = -1);

    ModelInfo model_info() const;
    bool initialized() const;
    void shutdown();
    const char* last_error() const;

private:
    struct Impl;
    Impl* impl_;
};

}  // namespace spacemit::stereo

#endif
