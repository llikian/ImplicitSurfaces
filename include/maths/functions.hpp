/***************************************************************************************************
 * @file  functions.hpp
 * @brief Declaration of maths functions
 **************************************************************************************************/

#pragma once

#include <cmath>
#include "vec3.hpp"

inline float pow2(float x) {
    return x * x;
}

inline float length2(const vec3& vec) {
    return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
}

inline vec3 min(const vec3& A, const vec3& B) {
    return vec3(std::min(A.x, B.x), std::min(A.y, B.y), std::min(A.z, B.z));
}

inline vec3 max(const vec3& A, const vec3& B) {
    return vec3(std::max(A.x, B.x), std::max(A.y, B.y), std::max(A.z, B.z));
}

inline float min_of(const vec3& vec) {
    return std::min({ vec.x, vec.y, vec.z });
}

inline float max_of(const vec3& vec) {
    return std::max({ vec.x, vec.y, vec.z });
}

inline vec3 floor(const vec3& vec) {
    return vec3(std::floor(vec.x), std::floor(vec.y), std::floor(vec.z));
}

inline vec3 ceil(const vec3& vec) {
    return vec3(std::ceil(vec.x), std::ceil(vec.y), std::ceil(vec.z));
}

vec3 hue_to_rgb(unsigned short hue);
