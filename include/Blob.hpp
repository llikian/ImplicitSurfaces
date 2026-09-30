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

    [[nodiscard]] virtual vec3 get_color(const vec3& /* point */) const = 0;

    float scale;
    AABB aabb;
};

struct SphereBlob : Blob {
    SphereBlob(float scale, const vec3& center, float radius);

    AABB get_aabb_and_blob_count(std::size_t& count) override;

    [[nodiscard]] float potential(const vec3& point) const override;

    [[nodiscard]] vec3 get_color(const vec3& /* point */) const override { return color; }

    vec3 color;

    vec3 center;

private:
    float radius;
    float radius_sqr;
};

struct CapsuleBlob : Blob {
    CapsuleBlob(float scale, const vec3& A, const vec3& B, float radius);

    AABB get_aabb_and_blob_count(std::size_t& count) override;

    [[nodiscard]] float potential(const vec3& point) const override;

    [[nodiscard]] vec3 get_color(const vec3& /* point */) const override { return color; }

    vec3 color;

    vec3 A;
    vec3 B;

private:
    float radius;
    float radius_sqr;
};

struct BoxBlob : Blob {
    BoxBlob(float scale, const vec3& center, const vec3& front, const vec3& right, float height);

    AABB get_aabb_and_blob_count(std::size_t& count) override;

    [[nodiscard]] float potential(const vec3& point) const override;

    [[nodiscard]] vec3 get_color(const vec3& /* point */) const override { return color; }

    vec3 color;

    vec3 center;
    vec3 front;
    vec3 right;

private:
    vec3 up;

    vec3 axis_x;
    vec3 axis_y;
    vec3 axis_z;
};

template <float PotentialFunc(float, float),       //
          AABB AABBFunc(const AABB&, const AABB&), //
          vec3 ColorFunc(const vec3&, float, const vec3&, float)>
struct OperationBlob : Blob {

    explicit OperationBlob(float scale) : Blob(scale), left(nullptr), right(nullptr) {}

    OperationBlob(float scale, Blob* left, Blob* right) : Blob(scale), left(left), right(right) {}

    AABB get_aabb_and_blob_count(std::size_t& count) override {
        ++count;

        aabb = AABBFunc(left->get_aabb_and_blob_count(count), right->get_aabb_and_blob_count(count));

        return aabb;
    }

    [[nodiscard]] float potential(const vec3& point) const override {
        return scale * PotentialFunc(left->potential(point), right->potential(point));
    }

    [[nodiscard]] vec3 get_color(const vec3& point) const override {
        return ColorFunc(left->get_color(point),
                         left->potential(point),
                         right->get_color(point),
                         right->potential(point));
    }

    Blob* left;
    Blob* right;
};

namespace PotentialFunctions {
    inline constexpr auto SUM = [](float a, float b) { return a + b; };
    inline constexpr auto MIN = [](float a, float b) { return std::min(a, b); };
    inline constexpr auto MAX = [](float a, float b) { return std::max(a, b); };
    inline constexpr auto DIFFERENCE = [](float a, float b) { return std::min(a, 2.0f * THRESHOLD - b); };
};

namespace ColorFunctions {
    inline constexpr auto BLEND = [](const vec3& a, float dist_a, const vec3& b, float dist_b) {
        float sum = dist_a + dist_b;
        if(sum < 0.0f) { return a; }
        return lerp(a, b, dist_b / sum);
    };
    inline constexpr auto UNION = [](const vec3& a, float dist_a, const vec3& b, float dist_b) {
        return dist_a < dist_b ? a : b;
    };
    inline constexpr auto INTERSECTION = [](const vec3& a, float dist_a, const vec3& b, float dist_b) {
        return dist_a > dist_b ? a : b;
    };
    inline constexpr auto DIFFERENCE = [](const vec3& a, float dist_a, const vec3& b, float dist_b) {
        return dist_a < (2.0f * THRESHOLD - dist_b) ? a : b;
    };
}

using BlendBlob = OperationBlob<PotentialFunctions::SUM, aabb_union, ColorFunctions::BLEND>;
using UnionBlob = OperationBlob<PotentialFunctions::MAX, aabb_union, ColorFunctions::UNION>;
using IntersectionBlob = OperationBlob<PotentialFunctions::MIN, aabb_intersect, ColorFunctions::INTERSECTION>;
using DifferenceBlob = OperationBlob<PotentialFunctions::DIFFERENCE, aabb_first, ColorFunctions::DIFFERENCE>;
