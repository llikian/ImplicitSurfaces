/***************************************************************************************************
 * @file  TransformBlob.cpp
 * @brief Implementation of the TransformBlob class
 **************************************************************************************************/

#include "blobs/TransformBlob.hpp"

std::unique_ptr<Blob> make_transform_blob(BlobTransformType transform_type, float scale) {
    switch(transform_type) {
        case BlobTransformType::translation: return std::make_unique<TranslationBlob>(scale);
        case BlobTransformType::twist:       return std::make_unique<TwistBlob>(scale); break;
    }

    return nullptr;
}

TransformBlob::TransformBlob(float scale) : Blob(scale), child(nullptr) {
}

[[nodiscard]] float TransformBlob::potential(const vec3& point) const {
    if(!aabb.is_point_inside(point)) {
        return 0.0f;
    }

    return scale * child->potential(transform(point));
}

[[nodiscard]] Sample TransformBlob::sample(const vec3& point) const {
    if(!aabb.is_point_inside(point)) {
        return Sample(0.0f, vec3(0.0f));
    }

    return child->sample(transform(point));
}

AABB TranslationBlob::get_aabb_and_blob_count(std::size_t& count) {
    ++count;

    aabb = child->get_aabb_and_blob_count(count) + translation;

    return aabb;
}

vec3 TranslationBlob::transform(const vec3& point) const {
    return point - translation;
}

AABB TwistBlob::get_aabb_and_blob_count(std::size_t& count) {
    ++count;

    aabb = child->get_aabb_and_blob_count(count);

    float x_sqr = std::max(pow2(aabb.pmin.x), pow2(aabb.pmax.x));
    float z_sqr = std::max(pow2(aabb.pmin.z), pow2(aabb.pmax.z));
    float radius = std::sqrt(x_sqr + z_sqr);

    aabb.pmin.x = aabb.pmin.z = -radius;
    aabb.pmax.x = aabb.pmax.z = radius;

    return aabb;
}

vec3 TwistBlob::transform(const vec3& point) const {
    float theta = rate * point.y;
    float cosine = std::cos(theta);
    float sine = std::sin(theta);
    return vec3(cosine * point.x + sine * point.z, point.y, -sine * point.x + cosine * point.z);
}
