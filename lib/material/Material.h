//
// Created by Leonard on 2025-01-24.
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include "../math/Ray.h"
#include "../path_tracing/RayStack.h"
#include "../render/LightSpectrum.h"



class RayStack;
class SceneObject;

/**
 * Represents a set of properties that determine how light is reflected,
 * refracted and transmitted through/off a surface.
 */
class Material {
private:
    const double roughness;
    const double minus_inv_attenuation_length;
    const double refractive_index;
    const LightSpectrum emittance;

public:
    /**
     * @param roughness Must be in the range [0, 1]. Weighs how much light is absorbed/emitted as opposed to
     * refracted/reflected.
     * @param attenuation_length Must be either @c -1 or non-negative. Determines the travel length at which light is
     * attenuated by a factor of 1/e. A value of @c -1 signifies no attenuation. A value of @c 0 signifies an
     * entirely opaque material.
     * @param refractive_index Must be non-negative. Determines how light is refracted when passing from another
     * material according to Snell's law.
     * @param emittance Determines how much light is emitted, scaled by @c roughness.
     */
    Material(double roughness, double attenuation_length, double refractive_index, const LightSpectrum& emittance);

    /**
     * Evaluates a light path by determining possible light paths from the point of incidence, and pushing them to the
     * ray stack to be evaluated at a later point.
     * @param object The object which was hit by the incident ray. Used for determining material properties and
     * resolving internal reflections.
     * @param ray_stack A ray stack to which evaluated light rays are pushed to.
     * @param incident_ray A ray with its @c from vector set to the point of incidence, and its @c dir vector set to the
     * incident light ray direction.
     * @param surface_normal The normalized surface normal vector at the point of incidence.
     * @return The light emitted by the object in the incident ray direction.
     */
    LightSpectrum eval_path(const SceneObject *object, RayStack &ray_stack,
        const Ray &incident_ray, const Vec3 &surface_normal) const;
};



#endif //MATERIAL_H
