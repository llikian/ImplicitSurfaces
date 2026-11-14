/***************************************************************************************************
 * @file  NENode.cpp
 * @brief Implementation of the NENode class
 **************************************************************************************************/

#include "engine/NENode.hpp"

#include "imgui_stdlib.h"

NENode::NENode(int id) : id(id), title_bar_color(IM_COL32(0, 0, 130, 255)) {
}

bool NENode::draw() {
    ImGui::PushItemWidth(200.0f);
    ImNodes::PushColorStyle(ImNodesCol_TitleBar, title_bar_color);
    ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL_DARKEN(title_bar_color, 0.8f));
    ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL_DARKEN(title_bar_color, 0.9f));
    ImNodes::BeginNode(id);

    ImNodes::BeginNodeTitleBar();
    ImGui::InputText("##Title", &title);
    ImNodes::EndNodeTitleBar();

    draw_attributes();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 5.0f));
    bool button_pressed = ImGui::Button("COMPUTE");
    ImGui::PopStyleVar();

    ImNodes::EndNode();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
    ImGui::PopItemWidth();

    return button_pressed;
}

Blob* NENode::get_blob() const {
    return nullptr;
};

NENodeSphereBlob::NENodeSphereBlob(int id, int& next_attribute_index, float scale, const vec3& center, float radius)
    : NENodeBlob(id, std::make_unique<SphereBlob>(scale, center, radius)),
      attr_blob(next_attribute_index++, this, IOType::OUT, "blob", IM_COL_MAGENTA, ImNodesPinShape_TriangleFilled) {
    title = "Sphere";
    title_bar_color = IM_COL_DARKEN(IM_COL_RED, 0.5f);
}

void NENodeSphereBlob::draw_attributes() {
    auto& b = *static_cast<SphereBlob*>(blob.get());

    ImGui::InputFloat("scale", &b.scale);
    ImGui::InputFloat3("center", &b.center.x);
    if(ImGui::InputFloat("radius", &b.radius)) {
        b.radius_sqr = b.radius * b.radius;
    }

    ImGui::ColorEdit3("color", &b.color.x);

    ImGui::NewLine();

    attr_blob.draw_pre();
    unformatted_text_aligned_right(attr_blob.name);
    attr_blob.draw_post();
}

void NENodeSphereBlob::get_attributes(std::vector<NEAttribute*>& attributes) {
    attributes.push_back(&attr_blob);
}

NENodeCapsuleBlob::NENodeCapsuleBlob(int id,
                                     int& next_attribute_index,
                                     float scale,
                                     const vec3& A,
                                     const vec3& B,
                                     float radius)
    : NENodeBlob(id, std::make_unique<CapsuleBlob>(scale, A, B, radius)),
      attr_blob(next_attribute_index++, this, IOType::OUT, "blob", IM_COL_MAGENTA, ImNodesPinShape_TriangleFilled) {
    title = "Capsule";
    title_bar_color = IM_COL_DARKEN(IM_COL_RED, 0.5f);
}

void NENodeCapsuleBlob::draw_attributes() {
    auto& b = *static_cast<CapsuleBlob*>(blob.get());

    ImGui::InputFloat("scale", &b.scale);
    ImGui::InputFloat3("start", &b.A.x);
    ImGui::InputFloat3("end", &b.B.x);
    if(ImGui::InputFloat("radius", &b.radius)) {
        b.radius_sqr = b.radius * b.radius;
    }

    ImGui::ColorEdit3("color", &b.color.x);

    ImGui::NewLine();

    attr_blob.draw_pre();
    unformatted_text_aligned_right(attr_blob.name);
    attr_blob.draw_post();
}

void NENodeCapsuleBlob::get_attributes(std::vector<NEAttribute*>& attributes) {
    attributes.push_back(&attr_blob);
}

NENodeOperationBlob::NENodeOperationBlob(int id, int& next_attribute_index, float blob_scale, BlobOpType op_type)
    : NENodeBlob(id, std::make_unique<OperationBlob>(blob_scale, op_type)),
      attr_blob_left(next_attribute_index++, this, IOType::IN, "left", IM_COL_MAGENTA, ImNodesPinShape_TriangleFilled),
      attr_blob_right(next_attribute_index++,
                      this,
                      IOType::IN,
                      "right",
                      IM_COL_MAGENTA,
                      ImNodesPinShape_TriangleFilled),
      attr_blob(next_attribute_index++, this, IOType::OUT, "blob", IM_COL_MAGENTA, ImNodesPinShape_TriangleFilled) {
    title = "Operation";
    title_bar_color = IM_COL_DARKEN(IM_COL_GREEN, 0.5f);
}

void NENodeOperationBlob::draw_attributes() {
    auto& b = *static_cast<OperationBlob*>(blob.get());

    ImGui::InputFloat("scale", &b.scale);

    ImGui::Text("Operation Type:");
    if(ImGui::RadioButton("Blend", b.op_type == BlobOpType::BLEND)) {
        b.op_type = BlobOpType::BLEND;
    }
    if(ImGui::RadioButton("Union", b.op_type == BlobOpType::UNION)) {
        b.op_type = BlobOpType::UNION;
    }
    if(ImGui::RadioButton("Intersection", b.op_type == BlobOpType::INTERSECTION)) {
        b.op_type = BlobOpType::INTERSECTION;
    }
    if(ImGui::RadioButton("Difference", b.op_type == BlobOpType::DIFFERENCE)) {
        b.op_type = BlobOpType::DIFFERENCE;
    }

    ImGui::NewLine();

    attr_blob_left.draw_pre();
    ImGui::TextUnformatted(attr_blob_left.name.c_str());
    attr_blob_left.draw_post();

    attr_blob_right.draw_pre();
    ImGui::TextUnformatted(attr_blob_right.name.c_str());
    attr_blob_right.draw_post();

    attr_blob.draw_pre();
    unformatted_text_aligned_right(attr_blob.name);
    attr_blob.draw_post();
}

void NENodeOperationBlob::get_attributes(std::vector<NEAttribute*>& attributes) {
    attributes.push_back(&attr_blob_left);
    attributes.push_back(&attr_blob_right);
    attributes.push_back(&attr_blob);
}

Blob* NENodeOperationBlob::get_blob() const {
    if(!attr_blob_left.linked_attribute || !attr_blob_right.linked_attribute) {
        return nullptr;
    }

    Blob* left = attr_blob_left.linked_attribute->node->get_blob();
    Blob* right = attr_blob_right.linked_attribute->node->get_blob();
    if(!left || !right) {
        return nullptr;
    }

    auto* b = static_cast<OperationBlob*>(blob.get());
    b->left = left;
    b->right = right;
    return b;
}
