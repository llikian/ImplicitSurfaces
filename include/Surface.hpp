/***************************************************************************************************
 * @file  Surface.hpp
 * @brief Declaration of the Surface class
 **************************************************************************************************/

#pragma once

#include <memory>
#include "blobs/Blob.hpp"
#include "mesh/Mesh.hpp"

static inline constexpr float EPSILON = 1e-3;

class Surface {
public:
    Surface() : root(nullptr) {}

    [[nodiscard]] float implicit(const vec3& point) const;
    [[nodiscard]] vec3 dichotomy(vec3 a, vec3 b, float va, float vb, float length) const;
    [[nodiscard]] vec3 gradient(const vec3& p) const;

    void compute_mesh(Mesh& mesh, int n, const AABB& region);

    Blob* root;
};
