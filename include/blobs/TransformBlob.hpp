/***************************************************************************************************
 * @file  TransformBlob.hpp
 * @brief Declaration of the TransformBlob class
 **************************************************************************************************/

#pragma once

#include <cstdint>
#include <memory>
#include "blobs/Blob.hpp"
#include "maths/constants.hpp"

enum class BlobTransformType : std::uint8_t {
    translation,
    rotation,
    twist,
    taper,
};

std::unique_ptr<Blob> make_transform_blob(BlobTransformType transform_type, float scale);

struct TransformBlob : Blob {
    explicit TransformBlob(float scale);

    void precompute(std::size_t& count) override;
    [[nodiscard]] float potential(const vec3& point) const override;
    [[nodiscard]] Sample sample(const vec3& point) const override;
    virtual bool transform(const vec3& point, vec3& transformed_point) const = 0;

    Blob* child;
};

struct TranslationBlob : TransformBlob {
    using TransformBlob::TransformBlob;

    void precompute(std::size_t& count) override;
    bool transform(const vec3& point, vec3& transformed_point) const override;

    vec3 translation;
};

struct RotationBlob : TransformBlob {
    using TransformBlob::TransformBlob;

    void precompute(std::size_t& count) override;
    bool transform(const vec3& point, vec3& transformed_point) const override;

    vec3 axis { 0.0f, 1.0f, 0.0f };
    float angle { 90.0f };

    vec3 columns[3];
};

// Paper: Global & Local Deformations of Solid Primitives - 1984 Alan H. Barr
struct TwistBlob : TransformBlob {
    using TransformBlob::TransformBlob;

    void precompute(std::size_t& count) override;
    bool transform(const vec3& point, vec3& transformed_point) const override;

    float rate { 1.0f };
};

struct TaperBlob : TransformBlob {
    using TransformBlob::TransformBlob;

    void precompute(std::size_t& count) override;
    bool transform(const vec3& point, vec3& transformed_point) const override;

    float rate { 1.0f };
};
