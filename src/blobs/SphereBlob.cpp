/***************************************************************************************************
 * @file  SphereBlob.cpp
 * @brief Implementation of the SphereBlob class
 **************************************************************************************************/

#include "blobs/SphereBlob.hpp"

SphereBlob::SphereBlob(float scale, const vec3& center, float radius, const vec3& color)
    : Blob(scale),
      color(color),
      center(center),
      radius(radius),
      radius_sqr(radius * radius) {
}

AABB SphereBlob::get_aabb_and_blob_count(std::size_t& count) {
    count++;

    aabb.pmin = center - radius;
    aabb.pmax = center + radius;

    return aabb;
}

[[nodiscard]] float SphereBlob::potential(const vec3& point) const {
    float distance_sqr = length2(point - center) / radius_sqr;
    if(distance_sqr >= 1.0f) {
        return 0.0f;
    }
    return scale * attenuation(distance_sqr);
}

[[nodiscard]] Sample SphereBlob::sample(const vec3& point) const {
    return Sample(potential(point), color);
}
