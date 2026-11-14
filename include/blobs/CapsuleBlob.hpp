/***************************************************************************************************
 * @file  CapsuleBlob.hpp
 * @brief Declaration of the CapsuleBlob class
 **************************************************************************************************/

#pragma once

#include "Blob.hpp"

struct CapsuleBlob : Blob {
    CapsuleBlob(float scale, const vec3& A, const vec3& B, float radius);

    void precompute(std::size_t& count) override;
    [[nodiscard]] float potential(const vec3& point) const override;
    [[nodiscard]] Sample sample(const vec3& point) const override;

    vec3 color;

    vec3 A;
    vec3 B;
    float radius;

private:
    vec3 AB;
    float inv_radius_sqr;
};
