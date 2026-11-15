/***************************************************************************************************
 * @file  AABB.hpp
 * @brief Declaration of the AABB class
 **************************************************************************************************/

#pragma once

#include "maths/functions.hpp"
#include "maths/mat4.hpp"
#include "maths/vec3.hpp"

struct AABB {
    vec3 pmin;
    vec3 pmax;

    bool is_point_inside(const vec3& point) const;
    vec3 get_center() const;
    mat4 get_global_model_matrix() const;
};

constexpr AABB aabb_union(const AABB& first, const AABB& second) {
    return AABB(min(first.pmin, second.pmin), max(first.pmax, second.pmax));
}

constexpr AABB aabb_intersect(const AABB& first, const AABB& second) {
    return AABB(max(first.pmin, second.pmin), min(first.pmax, second.pmax));
}

constexpr AABB aabb_first(const AABB& first, const AABB&) {
    return first;
}

constexpr AABB aabb_second(const AABB&, const AABB& second) {
    return second;
}
