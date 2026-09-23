/***************************************************************************************************
 * @file  Blob.hpp
 * @brief Declaration of the Blob class
 **************************************************************************************************/

#pragma once

#include "maths/geometry.hpp"
#include "maths/vec3.hpp"
#include "AABB.hpp"

#define THRESHOLD    0.5f
#define WYVILL_COUNT 2
#define GRID_SIZE    0.1f

float attenuation_wyvill(float distance_sqr, int n);
float aabb_radius(float radius);

struct Blob {
    explicit Blob(float scale);

    virtual ~Blob() = default;

    virtual AABB get_aabb_and_blob_count(std::size_t& count) = 0;

    [[nodiscard]] virtual float potential(const vec3& point) const = 0;

    float scale;
    AABB aabb;
};

struct SphereBlob : Blob {
    SphereBlob(float scale, const vec3& center, float radius);

    AABB get_aabb_and_blob_count(std::size_t& count) override;

    [[nodiscard]] float potential(const vec3& point) const override;

    vec3 center;

private:
    float radius;
    float radius_sqr;
};

struct CapsuleBlob : Blob {
    CapsuleBlob(float scale, const vec3& A, const vec3& B, float radius);

    AABB get_aabb_and_blob_count(std::size_t& count) override;

    [[nodiscard]] float potential(const vec3& point) const override;

    vec3 A;
    vec3 B;

private:
    float radius;
    float radius_sqr;
};

struct TwistBlobY : Blob {
    TwistBlobY(float scale, Blob* child, float angle) : Blob(scale), child(child), angle(angle) {}

    AABB get_aabb_and_blob_count(std::size_t& count) override {
        ++count;

        aabb = child->get_aabb_and_blob_count(count);
        aabb.pmin *= 2.0f;
        aabb.pmax *= 2.0f;

        return aabb;
    }

    vec3 twist(const vec3& point) const {
        float theta = angle * point.y;
        float cosine = std::cos(theta);
        float sine = std::sin(theta);

        return vec3(point.x * cosine - point.z * sine, //
                    point.y,
                    point.x * sine + point.z * cosine);
    }

    [[nodiscard]] float potential(const vec3& point) const override {
        if(!aabb.is_point_inside(point)) { return 0.0f; } // TODO : Benchmark with and without
        return scale * child->potential(twist(point));
    }

    Blob* child;
    float angle;
};

template <float PotentialFunc(float, float), //
          AABB AABBFunc(const AABB&, const AABB&)>
struct OperationBlob : Blob {

    explicit OperationBlob(float scale) : Blob(scale), left(nullptr), right(nullptr) {}

    OperationBlob(float scale, Blob* left, Blob* right) : Blob(scale), left(left), right(right) {}

    AABB get_aabb_and_blob_count(std::size_t& count) override {
        ++count;

        aabb = AABBFunc(left->get_aabb_and_blob_count(count), right->get_aabb_and_blob_count(count));
        return aabb;
    }

    [[nodiscard]] float potential(const vec3& point) const override {
        if(!aabb.is_point_inside(point)) { return 0.0f; } // TODO : Benchmark with and without

        return scale * PotentialFunc(left->potential(point), right->potential(point));
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

using BlendBlob = OperationBlob<PotentialFunctions::sum, aabb_union>;
using UnionBlob = OperationBlob<PotentialFunctions::max, aabb_union>;
using IntersectionBlob = OperationBlob<PotentialFunctions::min, aabb_intersect>;
using DifferenceBlob = OperationBlob<PotentialFunctions::difference, aabb_first>;
