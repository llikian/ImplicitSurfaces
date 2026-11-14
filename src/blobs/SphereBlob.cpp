/***************************************************************************************************
 * @file  SphereBlob.cpp
 * @brief Implementation of the SphereBlob class
 **************************************************************************************************/

#include "blobs/SphereBlob.hpp"

SphereBlob::SphereBlob(float scale, const vec3& center, float radius, const vec3& color)
    : Blob(scale),
      color(color),
      center(center),
      radius(radius) {
}

void SphereBlob::precompute(std::size_t& count) {
    Blob::precompute(count);

    aabb.pmin = center - radius;
    aabb.pmax = center + radius;

    inv_radius_sqr = 1.0f / (radius * radius);
}

[[nodiscard]] float SphereBlob::potential(const vec3& point) const {
    float distance_sqr = length2(point - center) * inv_radius_sqr;
    if(distance_sqr >= 1.0f) {
        return 0.0f;
    }
    return scale * attenuation(distance_sqr);
}

[[nodiscard]] Sample SphereBlob::sample(const vec3& point) const {
    return Sample(potential(point), color);
}
