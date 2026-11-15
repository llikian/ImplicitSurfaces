/***************************************************************************************************
 * @file  TransformBlob.cpp
 * @brief Implementation of the TransformBlob class
 **************************************************************************************************/

#include "blobs/TransformBlob.hpp"
#include "maths/geometry.hpp"
#include "maths/trigonometry.hpp"

std::unique_ptr<Blob> make_transform_blob(BlobTransformType transform_type, float scale) {
    switch(transform_type) {
        case BlobTransformType::translation: return std::make_unique<TranslationBlob>(scale);
        case BlobTransformType::rotation:    return std::make_unique<RotationBlob>(scale);
        case BlobTransformType::twist:       return std::make_unique<TwistBlob>(scale);
        case BlobTransformType::taper:       return std::make_unique<TaperBlob>(scale);
    }

    return nullptr;
}

TransformBlob::TransformBlob(float scale) : Blob(scale), child(nullptr) {
}

void TransformBlob::precompute(std::size_t& count) {
    Blob::precompute(count);
    child->precompute(count);
}

[[nodiscard]] float TransformBlob::potential(const vec3& point) const {
    if(!aabb.is_point_inside(point)) {
        return 0.0f;
    }

    vec3 transformed_point;
    if(!transform(point, transformed_point)) {
        return 0.0f;
    }

    return scale * child->potential(transformed_point);
}

[[nodiscard]] Sample TransformBlob::sample(const vec3& point) const {
    if(!aabb.is_point_inside(point)) {
        return Sample(0.0f, vec3(0.0f));
    }

    vec3 transformed_point;
    if(!transform(point, transformed_point)) {
        return Sample(0.0f, vec3(0.0f));
    }

    Sample child_sample = child->sample(transformed_point);
    child_sample.potential *= scale;
    return child_sample;
}

void TranslationBlob::precompute(std::size_t& count) {
    TransformBlob::precompute(count);

    aabb.pmin = child->aabb.pmin + translation;
    aabb.pmax = child->aabb.pmax + translation;
}

bool TranslationBlob::transform(const vec3& point, vec3& transformed_point) const {
    transformed_point = point - translation;
    return true;
}

void RotationBlob::precompute(std::size_t& count) {
    TransformBlob::precompute(count);

    float len = length2(axis);
    vec3 a = len > 1e-12f ? axis / std::sqrt(len) : vec3(0.0f, 1.0f, 0.0f);
    float rad = degrees_to_radians(angle);
    float c = std::cos(rad);
    float s = std::sin(rad);
    float t = 1.0f - c;

    columns[0] = vec3(t * a.x * a.x + c, t * a.x * a.y + s * a.z, t * a.x * a.z - s * a.y);
    columns[1] = vec3(t * a.x * a.y - s * a.z, t * a.y * a.y + c, t * a.y * a.z + s * a.x);
    columns[2] = vec3(t * a.x * a.z + s * a.y, t * a.y * a.z - s * a.x, t * a.z * a.z + c);

    aabb.pmin = vec3(INFINITY);
    aabb.pmax = vec3(-INFINITY);
    for(int i = 0; i < 8; ++i) {
        vec3 point = vec3((i & 1) ? child->aabb.pmax.x : child->aabb.pmin.x,
                          (i & 2) ? child->aabb.pmax.y : child->aabb.pmin.y,
                          (i & 4) ? child->aabb.pmax.z : child->aabb.pmin.z);

        point = columns[0] * point.x + columns[1] * point.y + columns[2] * point.z;

        aabb.pmin = min(aabb.pmin, point);
        aabb.pmax = max(aabb.pmax, point);
    }
}

bool RotationBlob::transform(const vec3& point, vec3& transformed_point) const {
    transformed_point = vec3(dot(columns[0], point), dot(columns[1], point), dot(columns[2], point));
    return true;
}

void TwistBlob::precompute(std::size_t& count) {
    TransformBlob::precompute(count);

    aabb = child->aabb;

    float x_sqr = std::max(pow2(aabb.pmin.x), pow2(aabb.pmax.x));
    float z_sqr = std::max(pow2(aabb.pmin.z), pow2(aabb.pmax.z));
    float radius = std::sqrt(x_sqr + z_sqr);

    aabb.pmin.x = aabb.pmin.z = -radius;
    aabb.pmax.x = aabb.pmax.z = radius;
}

bool TwistBlob::transform(const vec3& point, vec3& transformed_point) const {
    float theta = rate * point.y;
    float cosine = std::cos(theta);
    float sine = std::sin(theta);

    transformed_point = vec3(cosine * point.x + sine * point.z, point.y, -sine * point.x + cosine * point.z);

    return true;
}

void TaperBlob::precompute(std::size_t& count) {
    TransformBlob::precompute(count);

    aabb.pmin = vec3(INFINITY);
    aabb.pmax = vec3(-INFINITY);
    for(int i = 0; i < 8; ++i) {
        vec3 point = vec3((i & 1) ? child->aabb.pmax.x : child->aabb.pmin.x,
                          (i & 2) ? child->aabb.pmax.y : child->aabb.pmin.y,
                          (i & 4) ? child->aabb.pmax.z : child->aabb.pmin.z);
        float radius = 1.0f + rate * point.y;
        point = vec3(radius * point.x, point.y, radius * point.z);

        aabb.pmin = min(aabb.pmin, point);
        aabb.pmax = max(aabb.pmax, point);
    }
}

bool TaperBlob::transform(const vec3& point, vec3& transformed_point) const {
    float radius = 1.0f + rate * point.y;
    if(std::abs(radius) < 1e-4f) {
        return false;
    }

    transformed_point = vec3(point.x / radius, point.y, point.z / radius);

    return true;
}
