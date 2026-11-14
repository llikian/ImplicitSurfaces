/***************************************************************************************************
 * @file  CapsuleBlob.cpp
 * @brief Implementation of the CapsuleBlob class
 **************************************************************************************************/

#include "blobs/CapsuleBlob.hpp"

#include "maths/geometry.hpp"
#include "utility/Random.hpp"

CapsuleBlob::CapsuleBlob(float scale, const vec3& A, const vec3& B, float radius)
    : Blob(scale),
      color(Random::get_vec3(0.0f, 1.0f)),
      A(A),
      B(B),
      radius(radius),
      radius_sqr(radius * radius) {
}

AABB CapsuleBlob::get_aabb_and_blob_count(std::size_t& count) {
    count++;

    aabb.pmin = min(A, B) - radius;
    aabb.pmax = max(A, B) + radius;

    return aabb;
}

[[nodiscard]] float CapsuleBlob::potential(vec3 point) const {
    vec3 AB = B - A;
    float dist_AB_sqr = length2(AB);

    float t = (dist_AB_sqr > 0.0f) ? dot(point - A, AB) / dist_AB_sqr : 0.0f;
    t = std::clamp(t, 0.0f, 1.0f);

    float distance_sqr = length2(point - (A + t * AB)) / radius_sqr;
    if(distance_sqr >= 1.0f) {
        return 0.0f;
    }
    return scale * attenuation(distance_sqr);
}
