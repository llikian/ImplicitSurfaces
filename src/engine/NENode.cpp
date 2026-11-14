/***************************************************************************************************
 * @file  NENode.cpp
 * @brief Implementation of the NENode class
 **************************************************************************************************/

#include "engine/NENode.hpp"
#include "blobs/BezierBlob.hpp"

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
    ImGui::InputFloat("radius", &b.radius);

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
    ImGui::InputFloat("radius", &b.radius);
    ImGui::ColorEdit3("color", &b.color.x);

    ImGui::NewLine();

    attr_blob.draw_pre();
    unformatted_text_aligned_right(attr_blob.name);
    attr_blob.draw_post();
}

void NENodeCapsuleBlob::get_attributes(std::vector<NEAttribute*>& attributes) {
    attributes.push_back(&attr_blob);
}

NENodeOperationBlob::NENodeOperationBlob(int id, int& next_attribute_index, float scale, BlobOpType op_type)
    : NENodeBlob(id, nullptr),
      op_type(op_type),
      attr_blob_left(next_attribute_index++, this, IOType::IN, "left", IM_COL_MAGENTA, ImNodesPinShape_TriangleFilled),
      attr_blob_right(next_attribute_index++,
                      this,
                      IOType::IN,
                      "right",
                      IM_COL_MAGENTA,
                      ImNodesPinShape_TriangleFilled),
      attr_blob(next_attribute_index++, this, IOType::OUT, "blob", IM_COL_MAGENTA, ImNodesPinShape_TriangleFilled) {
    blob = make_op_blob(op_type, scale);
    title = "Operation";
    title_bar_color = IM_COL_DARKEN(IM_COL_GREEN, 0.5f);
}

void NENodeOperationBlob::draw_attributes() {
    auto& b = *static_cast<OperationBlob*>(blob.get());

    ImGui::Text("Operation Type:");
    if(ImGui::RadioButton("Blend", op_type == BlobOpType::BLEND)) {
        op_type = BlobOpType::BLEND;
        blob = make_op_blob(op_type, b.scale);
    }
    if(ImGui::RadioButton("Union", op_type == BlobOpType::UNION)) {
        op_type = BlobOpType::UNION;
        blob = make_op_blob(op_type, b.scale);
    }
    if(ImGui::RadioButton("Intersection", op_type == BlobOpType::INTERSECTION)) {
        op_type = BlobOpType::INTERSECTION;
        blob = make_op_blob(op_type, b.scale);
    }
    if(ImGui::RadioButton("Difference", op_type == BlobOpType::DIFFERENCE)) {
        op_type = BlobOpType::DIFFERENCE;
        blob = make_op_blob(op_type, b.scale);
    }

    ImGui::NewLine();

    ImGui::InputFloat("scale", &b.scale);

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
    if(attr_blob_left.linked_attribute == nullptr || attr_blob_right.linked_attribute == nullptr) {
        return nullptr;
    }

    Blob* left = attr_blob_left.linked_attribute->node->get_blob();
    Blob* right = attr_blob_right.linked_attribute->node->get_blob();
    if(left == nullptr || right == nullptr) {
        return nullptr;
    }

    auto* b = static_cast<OperationBlob*>(blob.get());
    b->left = left;
    b->right = right;
    return b;
}

NENodeTransformBlob::NENodeTransformBlob(int id,
                                         int& next_attribute_index,
                                         float scale,
                                         BlobTransformType transform_type)

    : NENodeBlob(id, nullptr),
      transform_type(transform_type),
      attr_blob_child(next_attribute_index++,
                      this,
                      IOType::IN,
                      "child",
                      IM_COL_MAGENTA,
                      ImNodesPinShape_TriangleFilled),
      attr_blob(next_attribute_index++, this, IOType::OUT, "blob", IM_COL_MAGENTA, ImNodesPinShape_TriangleFilled) {
    blob = make_transform_blob(transform_type, scale);
    title = "Transform";
    title_bar_color = IM_COL_DARKEN(IM_COL_BLUE, 0.5f);
}

void NENodeTransformBlob::draw_attributes() {
    auto b = static_cast<TransformBlob*>(blob.get());

    ImGui::Text("Transform Type:");
    if(ImGui::RadioButton("Translation", transform_type == BlobTransformType::translation)) {
        transform_type = BlobTransformType::translation;
        blob = make_transform_blob(transform_type, b->scale);
    }

    if(ImGui::RadioButton("Twist", transform_type == BlobTransformType::twist)) {
        transform_type = BlobTransformType::twist;
        blob = make_transform_blob(transform_type, b->scale);
    }

    ImGui::NewLine();

    ImGui::InputFloat("scale", &b->scale);
    switch(transform_type) {
        case BlobTransformType::translation:
            ImGui::InputFloat3("translation", &static_cast<TranslationBlob*>(b)->translation.x);
            break;
        case BlobTransformType::twist: ImGui::InputFloat("rate", &static_cast<TwistBlob*>(b)->rate); break;
    }

    ImGui::NewLine();

    attr_blob_child.draw_pre();
    ImGui::TextUnformatted(attr_blob_child.name.c_str());
    attr_blob_child.draw_post();

    attr_blob.draw_pre();
    unformatted_text_aligned_right(attr_blob.name);
    attr_blob.draw_post();
}

void NENodeTransformBlob::get_attributes(std::vector<NEAttribute*>& attributes) {
    attributes.push_back(&attr_blob_child);
    attributes.push_back(&attr_blob);
}

Blob* NENodeTransformBlob::get_blob() const {
    if(attr_blob_child.linked_attribute == nullptr) {
        return nullptr;
    }

    Blob* child = attr_blob_child.linked_attribute->node->get_blob();
    if(child == nullptr) {
        return nullptr;
    }

    auto* b = static_cast<TransformBlob*>(blob.get());
    b->child = child;
    return b;
}

NENodeBezierBlob::NENodeBezierBlob(int id, int& next_attribute_index, float scale)
    : NENodeBlob(id, std::make_unique<BezierBlob>(scale)),
      attr_blob(next_attribute_index++, this, IOType::OUT, "blob", IM_COL_MAGENTA, ImNodesPinShape_TriangleFilled) {
    title = "Bezier";
    title_bar_color = IM_COL_DARKEN(IM_COL_RED, 0.5f);
}

void NENodeBezierBlob::draw_attributes() {
    auto& b = *static_cast<BezierBlob*>(blob.get());

    ImGui::InputFloat("scale", &b.scale);

    ImGui::InputInt("spheres count", &b.spheres_count);

    ImGui::InputFloat3("p0", &b.p0.x);
    ImGui::InputFloat3("p1", &b.p1.x);
    ImGui::InputFloat3("p2", &b.p2.x);
    ImGui::InputFloat3("p3", &b.p3.x);

    ImGui::InputFloat("start radius", &b.radius_start);
    ImGui::InputFloat("end radius", &b.radius_end);

    ImGui::ColorEdit3("start color", &b.color_start.x);
    ImGui::ColorEdit3("end color", &b.color_end.x);

    ImGui::NewLine();

    attr_blob.draw_pre();
    unformatted_text_aligned_right(attr_blob.name);
    attr_blob.draw_post();
}

void NENodeBezierBlob::get_attributes(std::vector<NEAttribute*>& attributes) {
    attributes.push_back(&attr_blob);
}
