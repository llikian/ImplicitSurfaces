/***************************************************************************************************
 * @file  AABB.cpp
 * @brief Implementation of the AABB class
 **************************************************************************************************/

#include "AABB.hpp"

bool AABB::is_point_inside(const vec3& point) const {
    // clang-format off
    return point.x >= pmin.x && point.x <= pmax.x    
        && point.y >= pmin.y && point.y <= pmax.y
        && point.z >= pmin.z && point.z <= pmax.z;
    // clang-format on
}

vec3 AABB::get_center() const {
    // clang-format off
    return vec3(0.5f * (pmin.x + pmax.x),
                0.5f * (pmin.y + pmax.y),
                0.5f * (pmin.z + pmax.z));
    // clang-format on
}

mat4 AABB::get_global_model_matrix() const {
    vec3 center = get_center();

    // clang-format off
    return mat4(pmax.x - center.x, 0.0f, 0.0f, center.x,
                0.0f, pmax.y - center.y, 0.0f, center.y,
                0.0f, 0.0f, pmax.z - center.z, center.z,
                0.0f, 0.0f, 0.0f, 1.0f);
    // clang-format on
}
