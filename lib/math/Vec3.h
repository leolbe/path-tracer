//
// Created by Leonard on 2025-01-23.
//

#ifndef VEC3_H
#define VEC3_H

#include <string>

#include "Vec2.h"
#include "../util/Axis.h"



/**
 * A 3D mathematical vector.
 */
struct Vec3 {
    static const Vec3 ZERO, ONE, X, Y, Z, MIN, MAX;

    double x, y, z;

    Vec3(const Vec3 &v);
    Vec3(double x, double y, double z);
    Vec3(const Vec2 &xy, double z);
    Vec3(double x, const Vec2 &yz);
    explicit Vec3(double xyz);

    Vec3 operator+(const Vec3 &v) const;
    Vec3 operator+(double w) const;
    Vec3 operator-() const;
    Vec3 operator-(const Vec3 &v) const;
    Vec3 operator-(double w) const;
    Vec3 operator*(double d) const;
    Vec3 operator/(const Vec3 &v) const;
    Vec3 operator/(double d) const;
    double operator*(const Vec3 &v) const;

    double scalar_component(Axis axis) const;

    double sq() const;
    double magn() const;
    Vec3 norm() const;

    std::string debug_string() const;

    static Vec3 min(const Vec3&, const Vec3&);
    static Vec3 max(const Vec3&, const Vec3&);

    static Vec3 cross(const Vec3&, const Vec3&);
};



#endif //VEC3_H
