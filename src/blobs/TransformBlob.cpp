/***************************************************************************************************
 * @file  TransformBlob.cpp
 * @brief Implementation of the TransformBlob class
 **************************************************************************************************/

#include "blobs/TransformBlob.hpp"

std::unique_ptr<Blob> make_transform_blob(BlobTransformType transform_type, float scale) {
    switch(transform_type) {
        case BlobTransformType::translation: return std::make_unique<TranslationBlob>(scale);
        default:                             return nullptr;
    }
}

TransformBlob::TransformBlob(float scale) : Blob(scale), child(nullptr) {
}

AABB TranslationBlob::get_aabb_and_blob_count(std::size_t& count) {
    ++count;

    aabb = child->get_aabb_and_blob_count(count) + translation;

    return aabb;
}

[[nodiscard]] float TranslationBlob::potential(vec3 point) const {
    if(!aabb.is_point_inside(point)) {
        return 0.0f;
    }

    return scale * child->potential(point - translation);
}

[[nodiscard]] Sample TranslationBlob::sample(vec3 point) const {
    if(!aabb.is_point_inside(point)) {
        return Sample(0.0f, vec3(0.0f));
    }

    return child->sample(point - translation);
}
