/***************************************************************************************************
 * @file  NENode.hpp
 * @brief Declaration of the NENode class
 **************************************************************************************************/

#pragma once

#include <memory>
#include "blobs/CapsuleBlob.hpp"
#include "blobs/OperationBlob.hpp"
#include "blobs/SphereBlob.hpp"
#include "blobs/TransformBlob.hpp"
#include "imgui_internal.h"
#include "NEAttribute.hpp"

inline void unformatted_text_aligned_right(const std::string& text) {
    float item_width = ImGui::CalcItemWidth();
    float text_width = ImGui::CalcTextSize(text.c_str()).x;
    ImGui::Indent(std::max(0.0f, item_width - text_width + 50.0f)); // arbitrary but seems to work
    ImGui::TextUnformatted(text.c_str());
}

struct NENode {
    explicit NENode(int id);
    virtual ~NENode() = default;

    // bool value is for handling a button press
    bool draw();

    // bool value is for handling a button press
    virtual void draw_attributes() = 0;
    virtual void get_attributes(std::vector<NEAttribute*>& attributes) = 0;

    virtual Blob* get_blob() const;

    int id;
    std::string title;
    ImU32 title_bar_color;
};

struct NENodeBlob : NENode {
    NENodeBlob(int id, std::unique_ptr<Blob> blob) : NENode(id), blob(std::move(blob)) {}

    Blob* get_blob() const override { return blob.get(); }

    std::unique_ptr<Blob> blob;
};

struct NENodeSphereBlob : NENodeBlob {
    NENodeSphereBlob(int id, int& next_attribute_index, float scale, const vec3& center, float radius);

    void draw_attributes() override;
    void get_attributes(std::vector<NEAttribute*>& attributes) override;

    NEAttribute attr_blob;
};

struct NENodeCapsuleBlob : NENodeBlob {
    NENodeCapsuleBlob(int id, int& next_attribute_index, float scale, const vec3& A, const vec3& B, float radius);

    void draw_attributes() override;
    void get_attributes(std::vector<NEAttribute*>& attributes) override;

    NEAttribute attr_blob;
};

struct NENodeOperationBlob : NENodeBlob {
    NENodeOperationBlob(int id, int& next_attribute_index, float scale, BlobOpType op_type);

    void draw_attributes() override;
    void get_attributes(std::vector<NEAttribute*>& attributes) override;

    Blob* get_blob() const override;

    BlobOpType op_type;

    NEAttribute attr_blob_left;
    NEAttribute attr_blob_right;
    NEAttribute attr_blob;
};

struct NENodeTransformBlob : NENodeBlob {
    NENodeTransformBlob(int id, int& next_attribute_index, float scale, BlobTransformType transform_type);

    void draw_attributes() override;
    void get_attributes(std::vector<NEAttribute*>& attributes) override;

    Blob* get_blob() const override;

    BlobTransformType transform_type;

    NEAttribute attr_blob_child;
    NEAttribute attr_blob;
};
