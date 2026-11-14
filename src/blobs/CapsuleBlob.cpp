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
      inv_radius_sqr(1.0f / radius * radius) {
}

void CapsuleBlob::precompute(std::size_t& count) {
    Blob::precompute(count);

    aabb.pmin = min(A, B) - radius;
    aabb.pmax = max(A, B) + radius;

    AB = B - A;
    inv_radius_sqr = 1.0f / (radius * radius);
}

[[nodiscard]] float CapsuleBlob::potential(const vec3& point) const {
    float dist_AB_sqr = length2(AB);

    float t = (dist_AB_sqr > 0.0f) ? dot(point - A, AB) / dist_AB_sqr : 0.0f;
    t = std::clamp(t, 0.0f, 1.0f);

    float distance_sqr = length2(point - (A + t * AB)) * inv_radius_sqr;
    if(distance_sqr >= 1.0f) {
        return 0.0f;
    }
    return scale * attenuation(distance_sqr);
}

[[nodiscard]] Sample CapsuleBlob::sample(const vec3& point) const {
    return Sample(potential(point), color);
}
