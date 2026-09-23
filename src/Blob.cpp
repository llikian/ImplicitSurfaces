/***************************************************************************************************
 * @file  Blob.cpp
 * @brief Implementation of the Blob class
 **************************************************************************************************/

#include "Blob.hpp"

#include "maths/functions.hpp"
#include "maths/geometry.hpp"

float attenuation_wyvill(float distance_sqr, int n) {
    float base = 1.0f - distance_sqr;
    float result = 1.0f;

    for(int i = 0; i < n; ++i) { result *= base; }

    return result;
}

float aabb_radius(float radius) {
    // return radius * 0.75f;
    return radius;
}

Blob::Blob(float scale) : scale(scale) {}

SphereBlob::SphereBlob(float scale, const vec3& center, float radius)
    : Blob(scale),
      center(center),
      radius(radius),
      radius_sqr(radius * radius) {}

AABB SphereBlob::get_aabb_and_blob_count(std::size_t& count) {
    count++;

    float r = aabb_radius(radius);
    aabb = AABB(center - r, center + r);
    return aabb;
}

[[nodiscard]] float SphereBlob::potential(const vec3& point) const {
    float distance_sqr = length2(center - point) / radius_sqr;
    if(distance_sqr >= 1.0f) { return 0.0f; }
    return scale * attenuation_wyvill(distance_sqr, WYVILL_COUNT);
}

CapsuleBlob::CapsuleBlob(float scale, const vec3& A, const vec3& B, float radius)
    : Blob(scale),
      A(A),
      B(B),
      radius(radius),
      radius_sqr(radius * radius) {}

AABB CapsuleBlob::get_aabb_and_blob_count(std::size_t& count) {
    count++;
    float r = aabb_radius(radius);
    aabb = AABB(min(A, B) - r, max(A, B) + r);
    return aabb;
}

[[nodiscard]] float CapsuleBlob::potential(const vec3& point) const {
    vec3 AB = B - A;
    float dist_AB_sqr = length2(AB);

    float t = (dist_AB_sqr > 0.0f) ? dot(point - A, AB) / dist_AB_sqr : 0.0f;
    t = std::clamp(t, 0.0f, 1.0f);

    float distance_sqr = length2(point - (A + t * AB)) / radius_sqr;
    if(distance_sqr >= 1.0f) { return 0.0f; }
    return scale * attenuation_wyvill(distance_sqr, WYVILL_COUNT);
}
