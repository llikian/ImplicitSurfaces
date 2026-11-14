/***************************************************************************************************
 * @file  Blob.hpp
 * @brief Declaration of the Blob class
 **************************************************************************************************/

#pragma once

#include <functional>
#include "maths/vec3.hpp"
#include "AABB.hpp"

#define THRESHOLD 0.5f

float attenuation_wyvill(float distance_sqr, int n);
float attenuation_wyvill2(float distance_sqr);

#define attenuation(D) attenuation_wyvill2(D)

struct Sample {
    float potential;
    vec3 color;
};

struct Blob {
    explicit Blob(float scale);

    virtual ~Blob() = default;

    virtual AABB get_aabb_and_blob_count(std::size_t& count) = 0;

    [[nodiscard]] virtual float potential(vec3 point) const = 0;
    [[nodiscard]] virtual Sample sample(vec3 point) const = 0;

    float scale;

    AABB aabb;
};
