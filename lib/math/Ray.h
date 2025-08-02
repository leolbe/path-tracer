//
// Created by Leonard on 2025-01-23.
//

#ifndef RAY_H
#define RAY_H

#include "Vec3.h"



/**
 * Represents a 3D ray, with a starting point and direction.
 */
struct Ray {
    static const Ray X, Y, Z;

    Vec3 from, dir;

    Ray(const Vec3 &from, const Vec3 &dir);
};



#endif //RAY_H
