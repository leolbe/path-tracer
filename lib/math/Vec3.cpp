//
// Created by Leonard on 2025-01-23.
//

#include "Vec3.h"

#include <cmath>
#include <format>
#include <bits/max_size_type.h>



const Vec3 Vec3::ZERO = {0, 0, 0};
const Vec3 Vec3::ONE = {1, 1, 1};
const Vec3 Vec3::X = {1, 0, 0};
const Vec3 Vec3::Y = {0, 1, 0};
const Vec3 Vec3::Z = {0, 0, 1};
const Vec3 Vec3::MIN = Vec3(std::numeric_limits<double>::min());
const Vec3 Vec3::MAX = Vec3(std::numeric_limits<double>::max());

Vec3::Vec3(const Vec3 &v) = default;
Vec3::Vec3(const double x, const double y, const double z) : x(x), y(y), z(z) {}
Vec3::Vec3(const Vec2 &xy, const double z) : x(xy.x), y(xy.y), z(z) {}
Vec3::Vec3(const double x, const Vec2 &yz) : x(x), y(yz.x), z(yz.y) {}
Vec3::Vec3(const double xyz) : x(xyz), y(xyz), z(xyz) {}

Vec3 Vec3::operator+(const Vec3 &v) const {
    return {x + v.x, y + v.y, z + v.z};
}

Vec3 Vec3::operator+(const double w) const {
    return {x + w, y + w, z + w};
}

Vec3 Vec3::operator-() const {
    return {-x, -y, -z};
}


Vec3 Vec3::operator-(const Vec3 &v) const {
    return {x - v.x, y - v.y, z - v.z};
}

Vec3 Vec3::operator-(const double w) const {
    return {x - w, y - w, z - w};
}

Vec3 Vec3::operator*(const double d) const {
    return {x * d, y * d, z * d};
}

Vec3 Vec3::operator/(const Vec3 &v) const {
    return {x / v.x, y / v.y, z / v.z};
}


Vec3 Vec3::operator/(const double d) const {
    return {x / d, y / d, z / d};
}

double Vec3::operator*(const Vec3 &v) const {
    return x * v.x + y * v.y + z * v.z;
}

double Vec3::scalar_component(const Axis axis) const {
    switch (axis) {
    case Axis::X:
        return x;
    case Axis::Y:
        return y;
    default:
        return z;
    }
}

double Vec3::sq() const {
    return x*x + y*y + z*z;
}

double Vec3::magn() const {
    return std::sqrt(x*x + y*y + z*z);
}

Vec3 Vec3::norm() const {
    const double m = magn();
    return {x / m, y / m, z / m};
}

std::string Vec3::debug_string() const {
    return std::format("({}, {}, {})", x, y, z);
}

Vec3 Vec3::min(const Vec3 &v, const Vec3 &w) {
    return {std::min(v.x, w.x), std::min(v.y, w.y), std::min(v.z, w.z)};
}

Vec3 Vec3::max(const Vec3 &v, const Vec3 &w) {
    return {std::max(v.x, w.x), std::max(v.y, w.y), std::max(v.z, w.z)};
}

Vec3 Vec3::cross(const Vec3 &v, const Vec3 &w) {
    return {
        v.y * w.z - v.z * w.y,
        v.z * w.x - v.x * w.z,
        v.x * w.y - v.y * w.x};
}
