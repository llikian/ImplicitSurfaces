/***************************************************************************************************
 * @file  OperationBlob.cpp
 * @brief Implementation of the OperationBlob class
 **************************************************************************************************/

#include "blobs/OperationBlob.hpp"

std::unique_ptr<Blob> make_op_blob(BlobOpType op_type, float scale) {
    switch(op_type) {
        case BlobOpType::BLEND:        return std::make_unique<BlendBlob>(scale);
        case BlobOpType::UNION:        return std::make_unique<UnionBlob>(scale);
        case BlobOpType::INTERSECTION: return std::make_unique<IntersectionBlob>(scale);
        case BlobOpType::DIFFERENCE:   return std::make_unique<DifferenceBlob>(scale);
    }

    return nullptr;
}

OperationBlob::OperationBlob(float scale) : Blob(scale), left(nullptr), right(nullptr) {
}

void OperationBlob::precompute(std::size_t& count) {
    Blob::precompute(count);

    left->precompute(count);
    right->precompute(count);
}

void BlendBlob::precompute(std::size_t& count) {
    OperationBlob::precompute(count);

    aabb = aabb_union(left->aabb, right->aabb);
}

[[nodiscard]] float BlendBlob::potential(const vec3& point) const {
    if(!aabb.is_point_inside(point)) {
        return 0.0f;
    }

    return scale * left->potential(point) + right->potential(point);
}

[[nodiscard]] Sample BlendBlob::sample(const vec3& point) const {
    if(!aabb.is_point_inside(point)) {
        return Sample(0.0f, vec3(0.0f));
    }

    Sample sample_left = left->sample(point);
    Sample sample_right = right->sample(point);

    float abs_potential_right = std::abs(sample_right.potential);
    float sum = std::abs(sample_left.potential) + abs_potential_right;
    vec3 color;
    if(sum == 0.0f) {
        color = sample_left.color;
    } else {
        color = lerp(sample_left.color, sample_right.color, abs_potential_right / sum);
    }

    return Sample(scale * sample_left.potential + sample_right.potential, color);
}

void UnionBlob::precompute(std::size_t& count) {
    OperationBlob::precompute(count);

    aabb = aabb_union(left->aabb, right->aabb);
}

[[nodiscard]] float UnionBlob::potential(const vec3& point) const {
    if(!aabb.is_point_inside(point)) {
        return 0.0f;
    }

    return scale * std::max(left->potential(point), right->potential(point));
}

[[nodiscard]] Sample UnionBlob::sample(const vec3& point) const {
    if(!aabb.is_point_inside(point)) {
        return Sample(0.0f, vec3(0.0f));
    }

    Sample sample_left = left->sample(point);
    Sample sample_right = right->sample(point);

    if(sample_left.potential > sample_right.potential) {
        return Sample(scale * sample_left.potential, sample_left.color);
    } else {
        return Sample(scale * sample_right.potential, sample_right.color);
    }
}

void IntersectionBlob::precompute(std::size_t& count) {
    OperationBlob::precompute(count);

    aabb = aabb_intersect(left->aabb, right->aabb);
}

[[nodiscard]] float IntersectionBlob::potential(const vec3& point) const {
    if(!aabb.is_point_inside(point)) {
        return 0.0f;
    }

    return scale * std::min(left->potential(point), right->potential(point));
}

[[nodiscard]] Sample IntersectionBlob::sample(const vec3& point) const {
    if(!aabb.is_point_inside(point)) {
        return Sample(0.0f, vec3(0.0f));
    }

    Sample sample_left = left->sample(point);
    Sample sample_right = right->sample(point);

    if(sample_left.potential < sample_right.potential) {
        return Sample(scale * sample_left.potential, sample_left.color);
    } else {
        return Sample(scale * sample_right.potential, sample_right.color);
    }
}

void DifferenceBlob::precompute(std::size_t& count) {
    OperationBlob::precompute(count);

    aabb = left->aabb;
}

[[nodiscard]] float DifferenceBlob::potential(const vec3& point) const {
    if(!aabb.is_point_inside(point)) {
        return 0.0f;
    }

    return scale * std::min(left->potential(point), 2.0f * THRESHOLD - right->potential(point));
}

[[nodiscard]] Sample DifferenceBlob::sample(const vec3& point) const {
    if(!aabb.is_point_inside(point)) {
        return Sample(0.0f, vec3(0.0f));
    }

    Sample sample_left = left->sample(point);
    Sample sample_right = right->sample(point);

    if(left->potential(point) < 2.0f * THRESHOLD - right->potential(point)) {
        return Sample(scale * sample_left.potential, sample_left.color);
    } else {
        return Sample(scale * sample_right.potential, sample_right.color);
    }
}
