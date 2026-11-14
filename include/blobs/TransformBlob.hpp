/***************************************************************************************************
 * @file  TransformBlob.hpp
 * @brief Declaration of the TransformBlob class
 **************************************************************************************************/

#pragma once

#include <cstdint>
#include <memory>
#include "blobs/Blob.hpp"

enum class BlobTransformType : std::uint8_t {
    translation,
    twist,
};

std::unique_ptr<Blob> make_transform_blob(BlobTransformType transform_type, float scale);

struct TransformBlob : Blob {
    explicit TransformBlob(float scale);

    [[nodiscard]] float potential(vec3 point) const override;
    [[nodiscard]] Sample sample(vec3 point) const override;

    virtual vec3 transform(const vec3& point) const = 0;

    Blob* child;
};

struct TranslationBlob : TransformBlob {
    using TransformBlob::TransformBlob;

    AABB get_aabb_and_blob_count(std::size_t& count) override;
    vec3 transform(const vec3& point) const override;

    vec3 translation;
};

struct TwistBlob : TransformBlob {
    using TransformBlob::TransformBlob;

    AABB get_aabb_and_blob_count(std::size_t& count) override;
    vec3 transform(const vec3& point) const override;

    float rate;
};
