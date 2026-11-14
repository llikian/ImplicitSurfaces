/***************************************************************************************************
 * @file  OperationBlob.hpp
 * @brief Declaration of the OperationBlob class
 **************************************************************************************************/

#pragma once

#include <cstdint>
#include "Blob.hpp"

enum class BlobOpType : std::uint8_t { BLEND, UNION, INTERSECTION, DIFFERENCE };

inline float potential_of_op(BlobOpType op_type, float left, float right) {
    switch(op_type) {
        case BlobOpType::BLEND:        return left + right;
        case BlobOpType::UNION:        return std::max(left, right);
        case BlobOpType::INTERSECTION: return std::min(left, right);
        case BlobOpType::DIFFERENCE:   return std::min(left, 2.0f * THRESHOLD - right);
    }
    return 0.0f;
}

inline vec3 color_of_op(BlobOpType op_type, const Sample& left, const Sample& right) {
    switch(op_type) {
        case BlobOpType::BLEND: {
            float abs_potential_right = std::abs(right.potential);
            float sum = std::abs(left.potential) + abs_potential_right;
            if(sum <= 0.0f) {
                return left.color;
            }
            return lerp(left.color, right.color, abs_potential_right / sum);
        }
        case BlobOpType::UNION:        return left.potential > right.potential ? left.color : right.color;
        case BlobOpType::INTERSECTION: return left.potential < right.potential ? left.color : right.color;
        case BlobOpType::DIFFERENCE:
            return left.potential < (2.0f * THRESHOLD - right.potential) ? left.color : right.color;
    }

    return vec3(0.0f); // unreachable
}

struct OperationBlob : Blob {
    explicit OperationBlob(float scale, BlobOpType op_type);
    OperationBlob(float scale, BlobOpType op_type, Blob* left, Blob* right);

    AABB get_aabb_and_blob_count(std::size_t& count) override;

    [[nodiscard]] float potential(vec3 point) const override;
    [[nodiscard]] Sample sample(vec3 point) const override;

    BlobOpType op_type;
    Blob* left;
    Blob* right;
};
