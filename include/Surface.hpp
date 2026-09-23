/***************************************************************************************************
 * @file  Surface.hpp
 * @brief Declaration of the Surface class
 **************************************************************************************************/

#pragma once

#include <cstdint>
#include <vector>
#include "mesh/Mesh.hpp"
#include "AABB.hpp"
#include "Blob.hpp"

class Surface {
public:
    Surface() : root(nullptr) {}

    explicit Surface(Blob* root) : root(root) {}

    [[nodiscard]] float implicit(const vec3& point) const;

    [[nodiscard]] vec3 gradient(const vec3& pos) const;
    [[nodiscard]] vec3 normal(const vec3& pos) const;
    [[nodiscard]] vec3 dichotomy(vec3 a, vec3 b, float va, float vb, float length) const;

    [[nodiscard]] Mesh compute_mesh(int n);

    Blob* root;

    static inline constexpr float EPSILON = 1e-6;
};
