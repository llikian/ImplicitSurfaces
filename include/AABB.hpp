/***************************************************************************************************
 * @file  AABB.hpp
 * @brief Declaration of the AABB class
 **************************************************************************************************/

#pragma once

#include "maths/mat4.hpp"
#include "maths/vec3.hpp"

struct AABB {
    vec3 min;
    vec3 max;

    bool is_point_inside(const vec3& point) const;
    vec3 get_center() const;
    mat4 get_global_model_matrix() const;
};
