//
// Created by Leonard on 2025-01-24.
//

#include "Material.h"

#include <cassert>
#include <cmath>

#include "../math/math_util.h"
#include "../config.h"
#include "../util/debug_util.h"



Material::Material(const double roughness, const double attenuation_length, const double refractive_index,
    const LightSpectrum &emittance
)
  : roughness(roughness),
    minus_inv_attenuation_length(
        attenuation_length > 0
        ? -1 / attenuation_length
        : (attenuation_length == 0 ? -1 : 0) // 0 => opaque, set to -1. -1 => transparent, set to 0.
    ),
    refractive_index(refractive_index),
    emittance(emittance)
{
    if constexpr (DEBUG_ASSERTS) {
        assert(roughness >= 0 && roughness <= 1);
        assert(attenuation_length == -1 || attenuation_length >= 0);
        assert(refractive_index > 0);
    }
}

/**
 * Calculates the reflection coefficients for s and p polarized light.
 * For more information, see <a href="https://en.wikipedia.org/wiki/Fresnel_equations">Fresnel equations - Wikipedia</a>
 *
 * @param refractive_index_quotient = n_t / n_i, where n_t and n_i are the refractive indices of the material the light
 * passes to, and from, respectively.
 * @param cos_i = cos(θ_i), where θ_i is the incident light angle.
 * @param cos_t = cos(θ_t), where θ_t is the incident light angle.
 * @param out_R_s The reflection coefficient for s polarized light.
 * @param out_R_p The reflection coefficient for p polarized light.
 */
inline void fresnel_equations(const double refractive_index_quotient, const double cos_i, const double cos_t,
    double &out_R_s, double &out_R_p)
{
    if constexpr (DEBUG_ASSERTS) {
        assert(refractive_index_quotient > 0);
        assert(cos_i >= 0 && cos_i <= 1);
        assert(cos_t >= 0 && cos_t <= 1);
    }
    // Fresnel equations
    // (see https://en.wikipedia.org/wiki/Fresnel_equations)
    // R_s = ( (n_t cos_i - n_i cos_t) / (n_t cos_i + n_i cos_t) )²
    //     = ( ((n_t/n_i)cos_i - cos_t) / ((n_t/n_i)cos_i + cos_t) )²
    // R_p = ( (n_t cos_t - n_i cos_i) / (n_t cos_t + n_i cos_i) )²
    //     = ( ((n_t/n_i)cos_t - cos_i) / ((n_t/n_i)cos_t + cos_i) )²
    out_R_s = std::pow(
        (refractive_index_quotient * cos_i - cos_t) /
        (refractive_index_quotient * cos_i + cos_t),
        2);
    out_R_p = std::pow(
        (refractive_index_quotient * cos_t - cos_i) /
        (refractive_index_quotient * cos_t + cos_i),
        2);
}

/**
 * Calculates the refraction direction and the reflection coefficient.
 * See <a href="https://en.wikipedia.org/wiki/Schlick%27s_approximation">Schlick's approximation - Wikipedia</a> for
 * more information on Schlick's approximation, which can be toggled with the @c ENABLE_SCHLICKS_APPROXIMATION flag.
 *
 * @param refractive_index_quotient = n_t / n_i, where n_t and n_i are the refractive indices of the material the light
 * passes to, and from, respectively.
 * @param surface_normal The unit surface normal vector.
 * @param hit_dir The unit direction vector of a light ray.
 * @param out_refracted_dir The direction vector of incident light that would have been refracted in the @c hit_dir
 * direction. Not calculated if there is total internal reflection, as this wouldn't allow any light to have been
 * refracted.
 * @param out_reflection_coefficient Proportion of light that is reflected off the surface.
 */
void calc_refraction(const double refractive_index_quotient, const Vec3 &surface_normal, const Vec3 &hit_dir,
    Vec3 &out_refracted_dir, double &out_reflection_coefficient)
{
    if constexpr (DEBUG_ASSERTS) {
        assert(refractive_index_quotient > 0);
        debug_util::assert_unit(surface_normal);
        debug_util::assert_unit(hit_dir);
    }

    // Note that the t (transmitted) direction is in the direction of the hit ray, as the algorithm traces light
    // backwards. The i (incident) light direction, here called the refracted direction, is what is being calculated.

    // Let r denote the direction of light incident on the surface such that it is reflected in the hit_dir direction.
    // Let out_reflection_coefficient = R denote the proportion of incident light in the r direction that is reflected
    // off the surface, and T = 1 - R. T is thus the proportion of incident light in the r direction that is transmitted
    // through the surface. It can be shown that this value for T is equal to the transmitted coefficient, which
    // describes the proportion of transmitted light that is refracted from the inside of the material in the hit_dir
    // direction. Therefore, this value for the transmitted coefficient is used and only R is calculated here.

    const Vec3 negative_normal = (surface_normal * hit_dir > 0)
    ? surface_normal : -surface_normal;

    // Assumes incident_dir and negative_normal are normalized.
    const double cos_t = hit_dir * negative_normal;
    const double sin_t = std::sqrt(std::max(.0, 1 - cos_t * cos_t));
    // Snell's law, sin_i = sin_t * n_t / n_i
    const double sin_i = sin_t * refractive_index_quotient;
    if (sin_i > 1) {
        // total internal reflection
        out_reflection_coefficient = 1;
    } else {
        const double cos_i = std::sqrt(std::max(.0, 1 - sin_i * sin_i));

        // Find the components of the hit direction vector that are parallel/orthogonal to the normal direction vector.
        // |t_parallel| = cos_t, |t_orthogonal| = sin_t
        const Vec3 t_parallel = negative_normal * cos_t;
        const Vec3 t_orthogonal = hit_dir - t_parallel;

        // Find the components of the transmitted direction that are parallel/orthogonal to the normal direction vector.
        // |i_parallel| = cos_i, |i_orthogonal| = sin_i
        const Vec3 i_parallel = negative_normal * cos_i;
        // i_orthogonal points in the same direction as t_orthogonal as the hit and refraction direction vectors lie in
        // the same plane as the normal vector. Therefore, i_orthogonal * 1 / sin_i = t_orthogonal * 1 / sin_t.
        // (refractive_index_quotient = n_t / n_i = sin_i / sin_t)
        const Vec3 i_orthogonal = t_orthogonal * refractive_index_quotient;
        out_refracted_dir = i_parallel + i_orthogonal;

        if constexpr (ENABLE_POLARIZATION) {
            assert(false); // not implemented
        } else if constexpr (ENABLE_SCHLICKS_APPROXIMATION) {
            // Schlick's approximation
            // (see https://en.wikipedia.org/wiki/Schlick%27s_approximation)
            // R_0 = ( (n_i - n_t) / (n_i + n_t) )²
            //     = ( (1 - (n_t/n_i)) / (1 + (n_t/n_i)) )²
            // R ≈ R_0 + (1 - R_0)(1 - cos_i)⁵
            const double R_0 = std::pow((1 - refractive_index_quotient) / (1 + refractive_index_quotient), 2);
            out_reflection_coefficient = R_0 + (1 - R_0) * std::pow(1 - cos_i, 5);
        } else {
            // Otherwise, approximate the coefficient to the mean of R_s and R_p by assuming unpolarized light (slower
            // than Schlick's).
            double R_s;
            double R_p;
            fresnel_equations(refractive_index_quotient, cos_i, cos_t, R_s, R_p);
            out_reflection_coefficient = .5 * (R_s + R_p);
        }
    }
}

LightSpectrum Material::eval_path(const SceneObject *const object, RayStack &ray_stack, const Ray &incident_ray,
    const Vec3 &surface_normal) const
{
    if constexpr (DEBUG_SHADE_NORMALS)
        return LightSpectrum::from_rgb(surface_normal.x * .5 + .5, surface_normal.y * .5 + .5,
            surface_normal.z * .5 + .5, 1);

    const double smoothness = 1 - roughness;

    if (minus_inv_attenuation_length >= 0) {
        const Ray reflected_ray = {incident_ray.from, math_util::reflect(incident_ray.dir, surface_normal)};
        double reflection_factor = 0;
        Ray internal_ray = incident_ray;
        double internal_factor = 1;

        if (refractive_index != 1) {
            calc_refraction(1 / refractive_index, surface_normal, incident_ray.dir, internal_ray.dir,
                reflection_factor);
            internal_factor = 1 - reflection_factor;
        }

        reflection_factor *= smoothness;
        internal_factor *= smoothness;
        // also evaluate the light that would have been reflected towards hit_dir
        ray_stack.push(reflected_ray, LightTransformation::of_factor(reflection_factor));

        // the total distance traversed inside the material
        double internal_dist = 0;
        for (size_t reflections = 0;
            reflections < MAX_INTERNAL_REFLECTIONS && internal_factor > MIN_RAY_CONTRIBUTION;
            reflections++)
        {
            double delta_dist;
            Vec3 internal_hit_pos = incident_ray.from;
            Vec3 internal_hit_normal = surface_normal;
            object->ray_cast_from_inside(internal_ray, delta_dist, internal_hit_pos, internal_hit_normal);

            // refers to the direction of light that would have been transmitted into the material, which has "escaped"
            // from the evaluation of internal reflections
            Vec3 escape_dir = incident_ray.dir;
            double continued_internal_factor;
            calc_refraction(refractive_index, internal_hit_normal, internal_ray.dir, escape_dir,
                continued_internal_factor);
            const double escape_factor = 1 - continued_internal_factor;

            // each reflection off the inner surface contributes to the surface roughness attenuating the intensity
            internal_factor *= smoothness;
            internal_dist += delta_dist;
            if (escape_factor > 0 && internal_dist < std::numeric_limits<double>::infinity()) {
                const Ray escape_ray = {internal_hit_pos, escape_dir};
                // Beer-Lambert law:
                // I = I_0 exp(-μd) = I_0 exp(Ld)
                // Where L = -1/l, and l is the attenuation length given by
                // 1/e = I/I_0 = exp(-μl) ⇒
                // L = -1/l = -μ
                const double attenuation_factor = std::exp(minus_inv_attenuation_length * internal_dist);
                const double total_factor = internal_factor * escape_factor * attenuation_factor;
                // also evaluate light that has been reflected inside the material and refracted in the hit_dir
                // direction.
                ray_stack.push(escape_ray, LightTransformation::of_factor(total_factor), internal_dist,
                    reflections + 1);
            }

            // next, evaluate the ray that would have reflected off the inner surface, scaled by the reflection
            // coefficient
            internal_ray = {internal_hit_pos, math_util::reflect(internal_ray.dir, internal_hit_normal)};
            internal_factor *= continued_internal_factor;
        }
    } else if (roughness < 1) {
        // for non-transparent materials, evaluate the ray of light that would have reflected in the hit_dir direction.
        const Ray reflected_ray = {incident_ray.from, math_util::reflect(incident_ray.dir, surface_normal)};
        ray_stack.push(reflected_ray, LightTransformation::of_factor(smoothness));
    }
    return emittance.scaled(roughness);
}

