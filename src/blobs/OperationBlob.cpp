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
        default:                       return nullptr;
    }
}

OperationBlob::OperationBlob(float scale) : Blob(scale), left(nullptr), right(nullptr) {
}

AABB BlendBlob::get_aabb_and_blob_count(std::size_t& count) {
    ++count;

    aabb = aabb_union(left->get_aabb_and_blob_count(count), right->get_aabb_and_blob_count(count));

    return aabb;
}

[[nodiscard]] float BlendBlob::potential(vec3 point) const {
    if(!aabb.is_point_inside(point)) {
        return 0.0f;
    }

    return scale * left->potential(point) + right->potential(point);
}

[[nodiscard]] Sample BlendBlob::sample(vec3 point) const {
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

AABB UnionBlob::get_aabb_and_blob_count(std::size_t& count) {
    ++count;

    aabb = aabb_union(left->get_aabb_and_blob_count(count), right->get_aabb_and_blob_count(count));

    return aabb;
}

[[nodiscard]] float UnionBlob::potential(vec3 point) const {
    if(!aabb.is_point_inside(point)) {
        return 0.0f;
    }

    return scale * std::max(left->potential(point), right->potential(point));
}

[[nodiscard]] Sample UnionBlob::sample(vec3 point) const {
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

AABB IntersectionBlob::get_aabb_and_blob_count(std::size_t& count) {
    ++count;

    aabb = aabb_intersect(left->get_aabb_and_blob_count(count), right->get_aabb_and_blob_count(count));

    return aabb;
}

[[nodiscard]] float IntersectionBlob::potential(vec3 point) const {
    if(!aabb.is_point_inside(point)) {
        return 0.0f;
    }

    return scale * std::min(left->potential(point), right->potential(point));
}

[[nodiscard]] Sample IntersectionBlob::sample(vec3 point) const {
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

AABB DifferenceBlob::get_aabb_and_blob_count(std::size_t& count) {
    ++count;

    aabb = left->get_aabb_and_blob_count(count);
    right->get_aabb_and_blob_count(count);

    return aabb;
}

[[nodiscard]] float DifferenceBlob::potential(vec3 point) const {
    if(!aabb.is_point_inside(point)) {
        return 0.0f;
    }

    return scale * std::min(left->potential(point), 2.0f * THRESHOLD - right->potential(point));
}

[[nodiscard]] Sample DifferenceBlob::sample(vec3 point) const {
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
