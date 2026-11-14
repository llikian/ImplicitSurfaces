/***************************************************************************************************
 * @file  CapsuleBlob.hpp
 * @brief Declaration of the CapsuleBlob class
 **************************************************************************************************/

#pragma once

#include "Blob.hpp"

struct CapsuleBlob : Blob {
    CapsuleBlob(float scale, const vec3& A, const vec3& B, float radius);

    AABB get_aabb_and_blob_count(std::size_t& count) override;

    [[nodiscard]] float potential(vec3 point) const override;

    [[nodiscard]] Sample sample(vec3 point) const override { return Sample(potential(point), color); }

    vec3 color;

    vec3 A;
    vec3 B;
    float radius;
    float radius_sqr;
};
