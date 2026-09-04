// Stereo matching library usage example.

#include <stereo_matching.h>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

std::vector<float> PrepareInput(
    const cv::Mat& bgr, const spacemit::stereo::ModelInfo& info) {
    cv::Mat resized;
    cv::resize(bgr, resized,
               cv::Size(info.input_width, info.input_height),
               0.0, 0.0, cv::INTER_LINEAR);
    cv::Mat rgb;
    cv::cvtColor(resized, rgb, cv::COLOR_BGR2RGB);

    std::vector<float> input(info.input_elements);
    const std::size_t plane =
        static_cast<std::size_t>(info.input_width) * info.input_height;
    for (int y = 0; y < rgb.rows; ++y) {
        const auto* row = rgb.ptr<cv::Vec3b>(y);
        for (int x = 0; x < rgb.cols; ++x) {
            const std::size_t pixel =
                static_cast<std::size_t>(y) * rgb.cols + x;
            if (info.input_layout == spacemit::stereo::TensorLayout::nchw) {
                input[pixel] = row[x][0];
                input[plane + pixel] = row[x][1];
                input[2 * plane + pixel] = row[x][2];
            } else {
                input[3 * pixel] = row[x][0];
                input[3 * pixel + 1] = row[x][1];
                input[3 * pixel + 2] = row[x][2];
            }
        }
    }
    return input;
}

cv::Mat Colorize(const std::vector<float>& disparity,
                 const spacemit::stereo::ModelInfo& info) {
    float maximum = 0.0f;
    for (float value : disparity) {
        if (std::isfinite(value)) maximum = std::max(maximum, value);
    }

    cv::Mat gray(info.output_height, info.output_width, CV_8UC1, cv::Scalar(0));
    if (maximum > 0.0f) {
        for (int y = 0; y < info.output_height; ++y) {
            auto* row = gray.ptr<std::uint8_t>(y);
            for (int x = 0; x < info.output_width; ++x) {
                const float value = disparity[
                    static_cast<std::size_t>(y) * info.output_width + x];
                if (std::isfinite(value) && value > 0.0f) {
                    row[x] = static_cast<std::uint8_t>(
                        std::clamp(value / maximum * 255.0f, 0.0f, 255.0f));
                }
            }
        }
    }

    cv::Mat color;
    cv::applyColorMap(gray, color, cv::COLORMAP_TURBO);
    return color;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 4 || argc > 7) {
        std::cerr << "Usage: " << argv[0]
                  << " <model.onnx> <left.png> <right.png>"
                     " [output.png] [a100_cores] [instances]\n";
        return 2;
    }

    try {
        const std::string output =
            argc > 4 ? argv[4] : "/tmp/stereo_matching_output/disparity.png";

        cv::Mat left_image = cv::imread(argv[2], cv::IMREAD_COLOR);
        cv::Mat right_image = cv::imread(argv[3], cv::IMREAD_COLOR);
        if (left_image.empty() || right_image.empty()) {
            throw std::runtime_error("failed to load the stereo input images");
        }

        spacemit::stereo::Config config;
        config.model_path = argv[1];
        config.a100_core_count = argc > 5 ? std::stoi(argv[5]) : 8;
        config.instance_count = argc > 6 ? std::stoi(argv[6]) : 4;

        spacemit::stereo::StereoMatcher matcher;
        if (!matcher.initialize(config)) {
            throw std::runtime_error(matcher.last_error());
        }

        const auto info = matcher.model_info();
        auto left = PrepareInput(left_image, info);
        auto right = PrepareInput(right_image, info);
        std::vector<float> disparity(info.output_elements);

        const auto begin = std::chrono::steady_clock::now();
        if (!matcher.infer(left.data(), left.size(), right.data(), right.size(),
                           disparity.data(), disparity.size())) {
            throw std::runtime_error(matcher.last_error());
        }
        const double inference_ms = std::chrono::duration<double, std::milli>(
                                        std::chrono::steady_clock::now() - begin)
                                        .count();

        const std::filesystem::path output_path(output);
        if (output_path.has_parent_path()) {
            std::filesystem::create_directories(output_path.parent_path());
        }
        if (!cv::imwrite(output, Colorize(disparity, info))) {
            throw std::runtime_error("failed to save " + output);
        }

        std::cout << "input=" << info.input_width << 'x' << info.input_height
                  << " output=" << info.output_width << 'x' << info.output_height
                  << " inference_ms=" << inference_ms << '\n'
                  << "result=" << output << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << '\n';
        return 1;
    }
}
