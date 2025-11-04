//
// Created by Leonard on 2025-01-22.
//

#include <format>
#include <filesystem>
#include <iostream>

#include "scenes.h"
#include "../lib/render/RenderBuffer.h"
#include "../lib/scene/Scene.h"
#include "../lib/config.h"
#include "../lib/util/time_util.h"



int main() {
    auto scene = Scene(LightSpectrum::BLACK);
    Matrix4x4 projective_matrix;
    scenes::init_single_obj(scene, projective_matrix, "../obj/cow.obj");

    if constexpr (ENABLE_BVH_OPTIMIZATION) {
        time_util::start_stopwatch();
        scene.init();
        std::string init_time_string = time_util::format_stopwatch();
        std::cout << std::format("Initialized BVH in {}\n", init_time_string);
    }

    const auto buffer = std::make_unique<RenderBuffer>(OUTPUT_WIDTH, OUTPUT_HEIGHT, SAMPLE_GRID_SIZE);

    time_util::start_stopwatch();
    buffer->render(scene, projective_matrix, (V_FOV_DEGREES) / 180.0 * std::numbers::pi);
    std::string render_time_string = time_util::format_stopwatch();
    std::cout << std::format("Rendered scene in {}\n", render_time_string);

    const std::string output_path = buffer->write_png("./out/out.png");
    std::cout << std::format("Output file to:\n{}\n", output_path);
}