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

template <auto PotentialFunc, auto AABBFunc>
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

constexpr AABB aabb_union(const AABB& left, const AABB& right) {
    return AABB(min(left.min, right.min), max(left.max, right.max));
}

constexpr AABB aabb_intersect(const AABB& left, const AABB& right) {
    return AABB(max(left.min, right.min), min(left.max, right.max));
}

using SumBlob = OperationBlob<[](float a, float b) -> float { return a + b; }, aabb_union>;
using UnionBlob = OperationBlob<[](float a, float b) -> float { return std::max(a, b); }, aabb_union>;
using IntersectionBlob = OperationBlob<[](float a, float b) -> float { return std::min(a, b); }, aabb_intersect>;
using DifferenceBlob = OperationBlob<[](float a, float b) -> float { return std::min(a, 2.0f * THRESHOLD - b); },
                                     [](const AABB& left, const AABB&) -> AABB { return left; }>;
