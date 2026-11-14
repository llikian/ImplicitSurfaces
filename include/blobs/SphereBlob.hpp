/***************************************************************************************************
 * @file  SphereBlob.hpp
 * @brief Declaration of the SphereBlob class
 **************************************************************************************************/

#pragma once

#include "Blob.hpp"

#include "utility/Random.hpp"

struct SphereBlob : Blob {
    SphereBlob(float scale, const vec3& center, float radius, const vec3& color = Random::get_vec3(0.0f, 1.0f));

    void precompute(std::size_t& count) override;
    [[nodiscard]] float potential(const vec3& point) const override;
    [[nodiscard]] Sample sample(const vec3& point) const override;

    vec3 color;

    vec3 center;
    float radius;

private:
    float inv_radius_sqr;
};
