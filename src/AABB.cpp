/***************************************************************************************************
 * @file  AABB.cpp
 * @brief Implementation of the AABB class
 **************************************************************************************************/

#include "AABB.hpp"

bool AABB::is_point_inside(const vec3& point) const {
    return point.x >= min.x && point.x <= max.x    //
           && point.y >= min.y && point.y <= max.y //
           && point.z >= min.z && point.z <= max.z;
}

vec3 AABB::get_center() const {
    return vec3(0.5f * (min.x + max.x), 0.5f * (min.y + max.y), 0.5f * (min.z + max.z));
}

mat4 AABB::get_global_model_matrix() const {
    vec3 center = get_center();

    return mat4(max.x - center.x,
                0.0f,
                0.0f,
                center.x,
                0.0f,
                max.y - center.y,
                0.0f,
                center.y,
                0.0f,
                0.0f,
                max.z - center.z,
                center.z,
                0.0f,
                0.0f,
                0.0f,
                1.0f);
}
