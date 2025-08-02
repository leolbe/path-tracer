//
// Created by Leonard on 2025-01-23.
//

#ifndef VEC2_H
#define VEC2_H

#include <string>



/**
 * A 2D mathematical vector.
 */
struct Vec2 {
    static const Vec2 ZERO, ONE, X, Y;

    double x, y;

    Vec2(double x, double y);
    explicit Vec2(double xy);

    Vec2 operator+(Vec2 v) const;
    Vec2 operator+(double w) const;
    Vec2 operator-() const;
    Vec2 operator-(Vec2 v) const;
    Vec2 operator-(double w) const;
    Vec2 operator*(double d) const;
    Vec2 operator/(Vec2 v) const;
    Vec2 operator/(double d) const;
    double operator*(Vec2 v) const;

    double sq() const;
    double magn() const;
    Vec2 norm() const;

    std::string debug_string() const;

    static Vec2 min(const Vec2 &v, const Vec2 &w);
    static Vec2 max(const Vec2 &v, const Vec2 &w);
};



#endif //VEC2_H
