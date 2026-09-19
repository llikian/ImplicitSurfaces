/***************************************************************************************************
 * @file  Blob.hpp
 * @brief Declaration of the Blob class
 **************************************************************************************************/

#pragma once

#include "maths/functions.hpp"
#include "maths/vec3.hpp"
#include "AABB.hpp"

#define THRESHOLD    0.5f
#define WYVILL_COUNT 2
#define GRID_SIZE    0.1f

float attenuation_wyvill(float distance_sqr, int n);
float aabb_radius(float radius);

struct Blob {
    Blob();

    virtual ~Blob() = default;

    virtual AABB get_aabb_and_blob_count(std::size_t& count) = 0;

    [[nodiscard]] virtual float potential(const vec3& point) const = 0;

    bool negative;
};

struct SphereBlob : Blob {
    SphereBlob(const vec3& center, float radius);

    AABB get_aabb_and_blob_count(std::size_t& count) override;

    [[nodiscard]] float potential(const vec3& point) const override;

    vec3 center;

private:
    float radius;
    float radius_sqr;
};

struct CapsuleBlob : Blob {
    CapsuleBlob(const vec3& A, const vec3& B, float radius);

    AABB get_aabb_and_blob_count(std::size_t& count) override;

    [[nodiscard]] float potential(const vec3& point) const override;

    vec3 A;
    vec3 B;

private:
    float radius;
    float radius_sqr;
};

template <float PotentialFunc(float, float), //
          AABB AABBFunc(const AABB&, const AABB&)>
struct OperationBlob : Blob {

    OperationBlob() : left(nullptr), right(nullptr) {}

    OperationBlob(Blob* left, Blob* right) : left(left), right(right) {}

    AABB get_aabb_and_blob_count(std::size_t& count) override {
        ++count;

        return AABBFunc(left->get_aabb_and_blob_count(count), right->get_aabb_and_blob_count(count));
    }

    [[nodiscard]] float potential(const vec3& point) const override {
        return PotentialFunc(left->potential(point), right->potential(point));
    }

    Blob* left;
    Blob* right;
};

namespace PotentialFunctions {
    inline constexpr auto sum = [](float a, float b) { return a + b; };
    inline constexpr auto min = [](float a, float b) { return a < b ? a : b; };
    inline constexpr auto max = [](float a, float b) { return a > b ? a : b; };
    inline constexpr auto difference = [](float a, float b) { return a < b ? a : 2.0f * THRESHOLD - b; };
};

using SumBlob = OperationBlob<PotentialFunctions::sum, aabb_union>;
using UnionBlob = OperationBlob<PotentialFunctions::max, aabb_union>;
using IntersectionBlob = OperationBlob<PotentialFunctions::min, aabb_intersect>;
using DifferenceBlob = OperationBlob<PotentialFunctions::difference, aabb_first>;
