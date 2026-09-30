/***************************************************************************************************
 * @file  Blob.cpp
 * @brief Implementation of the Blob class
 **************************************************************************************************/

#include "Blob.hpp"

#include "maths/functions.hpp"
#include "maths/geometry.hpp"
#include "utility/Random.hpp"

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
      color(Random::get_vec3(0.0f, 1.0f)),
      center(center),
      radius(radius),
      radius_sqr(radius * radius) {}

AABB SphereBlob::get_aabb_and_blob_count(std::size_t& count) {
    count++;

    float r = aabb_radius(radius);
    aabb.pmin = center - r;
    aabb.pmax = center + r;

    return aabb;
}

[[nodiscard]] float SphereBlob::potential(const vec3& point) const {
    float distance_sqr = length2(center - point) / radius_sqr;
    if(distance_sqr >= 1.0f) { return 0.0f; }
    return scale * attenuation_wyvill(distance_sqr, WYVILL_COUNT);
}

CapsuleBlob::CapsuleBlob(float scale, const vec3& A, const vec3& B, float radius)
    : Blob(scale),
      color(Random::get_vec3(0.0f, 1.0f)),
      A(A),
      B(B),
      radius(radius),
      radius_sqr(radius * radius) {}

AABB CapsuleBlob::get_aabb_and_blob_count(std::size_t& count) {
    count++;

    float r = aabb_radius(radius);
    aabb.pmin = min(A, B) - r;
    aabb.pmax = max(A, B) + r;

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

BoxBlob::BoxBlob(float scale, const vec3& center, const vec3& front, const vec3& right, float height)
    : Blob(scale),
      color(Random::get_vec3(0.0f, 1.0f)),
      center(center),
      front(front),
      right(right),
      up(height * normalize(cross(front, right))),
      axis_x(right * (2.0f / length2(right))),
      axis_y(up * (2.0f / (height * height))),
      axis_z(front * (2.0f / length2(front))) {}

AABB BoxBlob::get_aabb_and_blob_count(std::size_t& count) {
    count++;

    vec3 dir = (front + right + up) * 0.5f;
    vec3 corner = center - dir;
    vec3 opposite_corner = center + dir;
    aabb.pmin = min(corner, opposite_corner);
    aabb.pmax = max(corner, opposite_corner);

    return aabb;
}

// Asked claude for the potential of a box
[[nodiscard]] float BoxBlob::potential(const vec3& point) const {
    const vec3 d = point - center;

    // superellipsoid, close enough to a box
    const float distance_sqr = pow8(dot(d, axis_x)) + pow8(dot(d, axis_y)) + pow8(dot(d, axis_z));
    if(distance_sqr >= 1.0f) { return 0.0f; }
    return scale * attenuation_wyvill(distance_sqr, WYVILL_COUNT);
}
