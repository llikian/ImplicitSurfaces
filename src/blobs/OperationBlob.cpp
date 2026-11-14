/***************************************************************************************************
 * @file  OperationBlob.cpp
 * @brief Implementation of the OperationBlob class
 **************************************************************************************************/

#include "blobs/OperationBlob.hpp"

OperationBlob::OperationBlob(float scale, BlobOpType op_type)
    : Blob(scale),
      op_type(op_type),
      left(nullptr),
      right(nullptr) {
}

OperationBlob::OperationBlob(float scale, BlobOpType op_type, Blob* left, Blob* right)
    : Blob(scale),
      op_type(op_type),
      left(left),
      right(right) {
}

AABB OperationBlob::get_aabb_and_blob_count(std::size_t& count) {
    ++count;

    aabb = left->get_aabb_and_blob_count(count);
    AABB aabb_right = right->get_aabb_and_blob_count(count);

    switch(op_type) {
        case BlobOpType::BLEND:
        case BlobOpType::UNION:        aabb = aabb_union(aabb, aabb_right); break;
        case BlobOpType::INTERSECTION: aabb = aabb_intersect(aabb, aabb_right); break;
        case BlobOpType::DIFFERENCE:   break;
    }

    return aabb;
}

[[nodiscard]] float OperationBlob::potential(vec3 point) const {
    if(!aabb.is_point_inside(point)) {
        return 0.0f;
    }

    return scale * potential_of_op(op_type, left->potential(point), right->potential(point));
}

[[nodiscard]] Sample OperationBlob::sample(vec3 point) const {
    if(!aabb.is_point_inside(point)) {
        return Sample(0.0f, vec3(0.0f));
    }

    Sample sample_left = left->sample(point);
    Sample sample_right = right->sample(point);

    return Sample(scale * potential_of_op(op_type, sample_left.potential, sample_right.potential),
                  color_of_op(op_type, sample_left, sample_right));
}
