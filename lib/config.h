//
// Created by Leonard on 2025-02-03.
//

#ifndef CONFIG_H
#define CONFIG_H

#include <cstddef>



//////// OUTPUT QUALITY ////////

inline constexpr size_t OUTPUT_WIDTH = 1920;

inline constexpr size_t OUTPUT_HEIGHT = 1080;

// the size of the square grid of samples taken per pixel
inline constexpr size_t SAMPLE_GRID_SIZE = 1;



//////// CAMERA SETTINGS ////////

// vertical field of view
inline constexpr size_t V_FOV_DEGREES = 60;



//////// PATH TRACING COMPLEXITY ////////

// limits how deep (the amount of surface interactions) the path tracing
// algorithm will search for light sources for a single ray
inline constexpr size_t DEPTH_LIMIT = 10;

// discard light rays that contribute less than this factor to a single sample
inline constexpr double MIN_RAY_CONTRIBUTION = 0.01;

// limits the depth of reflections that are evaluated within an object.
// effectively limited by DEPTH_LIMIT.
inline constexpr size_t MAX_INTERNAL_REFLECTIONS = 8;

// determines the max value with which an asserted value may deviate
inline constexpr double TOLERANCE = 0.00001;

// enable using Schlick's approximation for calculating refraction
// (see https://en.wikipedia.org/wiki/Schlick%27s_approximation)
inline constexpr bool ENABLE_SCHLICKS_APPROXIMATION = true;



//////// FEATURES ////////

// simulate light polarization state
inline constexpr bool ENABLE_POLARIZATION = false;

// simulate differences in diffraction of different frequencies of light
inline constexpr bool ENABLE_DISPERSION = false;



//////// DEBUG ////////

// enable state-checking asserts
inline constexpr bool DEBUG_ASSERTS = true;

// color all pixels with the normal direction of the hit surface
inline constexpr bool DEBUG_SHADE_NORMALS = false;



#endif //CONFIG_H
