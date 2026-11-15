/***************************************************************************************************
 * @file  BezierBlob.hpp
 * @brief Declaration of the BezierBlob class
 **************************************************************************************************/

#pragma once

#include "Blob.hpp"

struct BezierBlob : Blob {
    using Blob::Blob;

    void precompute(std::size_t& count) override;
    [[nodiscard]] float potential(const vec3& point) const override;
    [[nodiscard]] Sample sample(const vec3& point) const override;

    int spheres_count { 16 };
    vec3 p0, p1, p2, p3;
    float radius_start { 1.0f };
    float radius_end { 1.0f };
    vec3 color_start { 0.0f, 0.0f, 0.0f };
    vec3 color_end { 1.0f, 1.0f, 1.0f };

private:
    vec3 evaluate(float t);

    struct PrecomputedValue {
        vec3 center;
        float inv_radius_sqr;
        vec3 color;
    };

    std::vector<PrecomputedValue> precomputed_values;
};
