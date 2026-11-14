/***************************************************************************************************
 * @file  OperationBlob.hpp
 * @brief Declaration of the OperationBlob class
 **************************************************************************************************/

#pragma once

#include <cstdint>
#include <memory>
#include "Blob.hpp"

enum class BlobOpType : std::uint8_t { BLEND, UNION, INTERSECTION, DIFFERENCE };

std::unique_ptr<Blob> make_op_blob(BlobOpType op_type, float scale);

struct OperationBlob : Blob {
    explicit OperationBlob(float scale);

    void precompute(std::size_t& count) override;

    Blob* left;
    Blob* right;
};

struct BlendBlob : OperationBlob {
    using OperationBlob::OperationBlob;

    void precompute(std::size_t& count) override;
    [[nodiscard]] float potential(const vec3& point) const override;
    [[nodiscard]] Sample sample(const vec3& point) const override;
};

struct UnionBlob : OperationBlob {
    using OperationBlob::OperationBlob;

    void precompute(std::size_t& count) override;
    [[nodiscard]] float potential(const vec3& point) const override;
    [[nodiscard]] Sample sample(const vec3& point) const override;
};

struct IntersectionBlob : OperationBlob {
    using OperationBlob::OperationBlob;

    void precompute(std::size_t& count) override;
    [[nodiscard]] float potential(const vec3& point) const override;
    [[nodiscard]] Sample sample(const vec3& point) const override;
};

struct DifferenceBlob : OperationBlob {
    using OperationBlob::OperationBlob;

    void precompute(std::size_t& count) override;
    [[nodiscard]] float potential(const vec3& point) const override;
    [[nodiscard]] Sample sample(const vec3& point) const override;
};
