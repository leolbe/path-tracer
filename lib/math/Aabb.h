//
// Created by Leonard on 2025-01-24.
//

#ifndef AABB_H
#define AABB_H

#include "Ray.h"
#include "Vec3.h"



/**
 * Axis-Aligned Bounding Box.
 */
struct Aabb {
    static const Aabb UNIT;

    Vec3 min, max;

    /**
     * @param min The corner of the box with the minimal x, y, and z coordinate.
     * @param max The corner of the box with the maximal x, y, and z coordinate.
     */
    Aabb(const Vec3 &min, const Vec3 &max);

    bool contains(const Vec3 &pos) const;

    bool intersects(const Ray &ray) const;

    /**
     * @return The distance from the ray's origin to its intersection with this
     * AABB, or @c -1 if it doesn't intersect.
     */
    double ray_length(const Ray &ray) const;
};



#endif //AABB_H
