//
// Created by Leonard on 2025-02-01.
//

#include "scenes.h"

#include "../lib/scene/scene_objects/BoxObject.h"
#include "../lib/scene/scene_objects/MeshObject.h"
#include "../lib/scene/scene_objects/SphereObject.h"
#include "../lib/scene/scene_objects/mesh_util.h"



static const auto white_material = Material(1, 0, 1, LightSpectrum::from_rgb(1, 1, 1, 1));
static const auto red_material = Material(.9, 0, 1, LightSpectrum::from_rgb(1, 0, 0, 1));
static const auto green_material = Material(.9, 0, 1, LightSpectrum::from_rgb(0, 1, 0, 1));
static const auto blue_material = Material(.9, 0, 1, LightSpectrum::from_rgb(0, 0, 1, 1));
static const auto cyan_material = Material(.9, 0, 1, LightSpectrum::from_rgb(0, 1, 1, 1));
static const auto magenta_material = Material(.9, 0, 1, LightSpectrum::from_rgb(1, 0, 1, 1));
static const auto yellow_material = Material(.9, 0, 1, LightSpectrum::from_rgb(1, 1, 0, 1));

static const auto metal_material = Material(.1, 0, 1, LightSpectrum());
static const auto glass_material = Material(.15, -1, 1.5, LightSpectrum());
static const auto non_refractive_glass_material = Material(.1, -1, 1, LightSpectrum());
static const auto translucent_material = Material(.3, 2, 1.2, LightSpectrum::from_rgb(0, .7, 0, 1));

static const auto completely_transparent_material = Material(0, -1, 1, LightSpectrum());
static const auto completely_reflective_material = Material(0, 0, 1, LightSpectrum());
static const auto transparent_reflective_material = Material(.5, -1, 1.2, LightSpectrum::from_rgb(1, 1, 1, 1));

void scenes::init_refraction_demo(Scene &scene, Matrix4x4 &projective_matrix) {
    scene.add_object(std::make_unique<SphereObject>(Vec3(0, 1000020, 0), 1000000, &white_material));
    scene.add_object(std::make_unique<SphereObject>(Vec3(-5, 0, 0), 4, &magenta_material));
    scene.add_object(std::make_unique<SphereObject>(Vec3(5, 0, 0), 4, &cyan_material));
    scene.add_object(std::make_unique<SphereObject>(Vec3(-2, -3, -7), 1.5, &transparent_reflective_material));
    projective_matrix = Matrix4x4(
        Matrix3x3::from_forward_down_vecs(
            Vec3(1, 2, 3).norm(),
            Vec3::Y
        ),
        Vec3(-3, -6, -9)
    );
}

void scenes::init_translucency_demo(Scene &scene, Matrix4x4 &projective_matrix) {
    scene.add_object(std::make_unique<SphereObject>(Vec3(0, 1000020, 0), 1000000, &white_material));
    scene.add_object(std::make_unique<SphereObject>(Vec3(-5, 0, 0), 4, &yellow_material));
    scene.add_object(std::make_unique<SphereObject>(Vec3(5, 0, 0), 4, &red_material));
    scene.add_object(std::make_unique<SphereObject>(Vec3(-2, -3, -7), 1.5, &translucent_material));
    projective_matrix = Matrix4x4(
        Matrix3x3::from_forward_down_vecs(
            Vec3(1, 2, 3).norm(),
            Vec3::Y
        ),
        Vec3(-3, -6, -9)
    );
}


void scenes::init_colored_sphere_grid(Scene &scene, Matrix4x4 &projective_matrix) {
    scene.add_object(std::make_unique<SphereObject>(Vec3(0, 1000020, 0), 1000000, &white_material));
    constexpr int M = 3;
    for (int i = -M; i <= M; i++) {
        for (int j = -M; j <= M; j++) {
            for (int k = -M; k <= M; k++) {
                const int mat = rand() % 3;
                const Material *material;
                if ((i + j + k) % 2 == 0) {
                    material = &glass_material;
                } else if (mat == 0) {
                    material = &cyan_material;
                } else if (mat == 1) {
                    material = &magenta_material;
                } else {
                    material = &yellow_material;
                }
                scene.add_object(std::make_unique<SphereObject>(Vec3(i, j, k), .35, material));
            }
        }
    }
    projective_matrix = Matrix4x4(
        Matrix3x3::from_forward_down_vecs(
            Vec3(1, 2, 3).norm(),
            Vec3::Y
        ),
        Vec3(-3, -6, -9)
    );
}

void scenes::init_glass_grid(Scene &scene, Matrix4x4 &projective_matrix) {
    scene.add_object(std::make_unique<SphereObject>(Vec3(0, 1000020, 0), 1000000, &white_material));
    scene.add_object(std::make_unique<SphereObject>(Vec3(0, 0, 10), 3, &cyan_material));
    scene.add_object(std::make_unique<SphereObject>(Vec3(1, 1, 5), .8, &transparent_reflective_material));
    scene.add_object(std::make_unique<SphereObject>(Vec3(1, -1, 5), .8, &transparent_reflective_material));
    scene.add_object(std::make_unique<SphereObject>(Vec3(-1, 1, 5), .8, &transparent_reflective_material));
    scene.add_object(std::make_unique<SphereObject>(Vec3(-1, -1, 5), .8, &transparent_reflective_material));
    constexpr int N = 5;
    for (int i = -N; i <= N; i++) {
        for (int j = -N; j <= N; j++) {
            scene.add_object(std::make_unique<SphereObject>(Vec3((i + j) * .22, (i -j) * .22, 3), .15,
                &glass_material));
        }
    }
    projective_matrix = Matrix4x4(Matrix3x3::IDENT, Vec3::ZERO);
}

void scenes::init_box_grid(Scene &scene, Matrix4x4 &projective_matrix) {
    scene.add_object(std::make_unique<BoxObject>(Aabb({-1000000, 20, -1000000}, {1000000, 1000000, 1000000}),
        &white_material));
    constexpr int M = 1;
    for (int i = -M; i <= M; i++) {
        for (int j = -M; j <= M; j++) {
            for (int k = -M; k <= M; k++) {
                const Material *material;
                if ((i + j + k) % 2 == 0) {
                    material = &glass_material;
                } else {
                    material = &metal_material;
                }
                scene.add_object(std::make_unique<BoxObject>(Aabb({i - .2, j - .2, k - .2}, {i + .2, j + .2, k + .2}),
                    material));
            }
        }
    }
    projective_matrix = Matrix4x4(
        Matrix3x3::from_forward_down_vecs(
            Vec3(1, 2, 3).norm(),
            Vec3::Y
        ),
        Vec3(-1, -2, -3)
    );
}

void scenes::init_glass_boxes(Scene &scene, Matrix4x4 &projective_matrix) {
    scene.add_object(std::make_unique<BoxObject>(Aabb({-1000000, 20, -1000000},{1000000, 1000000, 1000000}),
        &white_material));
    scene.add_object(std::make_unique<BoxObject>(Aabb({-9, -4, -4}, {-1, 4, 4}), &magenta_material));
    scene.add_object(std::make_unique<BoxObject>(Aabb({1, -4, -4}, {9, 4, 4}), &cyan_material));
    scene.add_object(std::make_unique<BoxObject>(Aabb({-3.5, -4.5, -8.5}, {-.5, -1.5, -5.5}), &translucent_material));
    projective_matrix = Matrix4x4(
        Matrix3x3::from_forward_down_vecs(
            Vec3(1, 2, 3).norm(),
            Vec3::Y
        ),
        Vec3(-5, -10, -15)
    );
}

void scenes::init_pyramid_mesh(Scene &scene, Matrix4x4 &projective_matrix) {
    std::vector<Vec3> tri_vertices = {
        {1, 1, -1}, {-1, 1, -1}, {0, -1, 0},
        {1, 1, 1}, {1, 1, -1}, {0, -1, 0},
        {-1, 1, 1}, {1, 1, 1}, {0, -1, 0},
        {-1, 1, -1}, {-1, 1, 1}, {0, -1, 0},
        {-1, 1, -1}, {1, 1, 1}, {-1, 1, 1},
        {1, 1, 1}, {-1, 1, -1}, {1, 1, -1},
    };
    scene.add_object(std::make_unique<MeshObject>(std::move(tri_vertices), &translucent_material));
    scene.add_object(std::make_unique<SphereObject>(Vec3(0, 1000020, 0), 1000000, &white_material));
    projective_matrix = Matrix4x4(
        Matrix3x3::from_forward_down_vecs(
            Vec3(1, 2, 3).norm(),
            Vec3::Y
        ),
        Vec3(-1, -2, -3)
    );
}

bool scenes::init_single_obj(Scene &scene, Matrix4x4 &projective_matrix, const std::string &obj_path) {
    auto mesh = mesh_util::from_obj(obj_path, &glass_material);
    if (!mesh.has_value())
        return false;
    scene.add_object(std::move(mesh.value()));
    scene.add_object(std::make_unique<SphereObject>(Vec3(0, 1000020, 0), 1000000, &white_material));
    projective_matrix = Matrix4x4(
        Matrix3x3::from_forward_down_vecs(
            Vec3(-3, -2, 3).norm(),
            -Vec3::Y
        ),
        Vec3(9, 6, -9)
    );
    return true;
}
