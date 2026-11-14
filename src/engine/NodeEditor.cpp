/***************************************************************************************************
 * @file  NodeEditor.cpp
 * @brief Implementation of the NodeEditor class
 **************************************************************************************************/

#include "engine/NodeEditor.hpp"

#include "imnodes.h"

NodeEditor::NodeEditor()
    : next_node_index(0),
      next_attribute_index(0),
      next_link_id(0),
      link_color(IM_COL32_WHITE),
      add_node_start_attribute(-1),
      right_click_node_id(-1),
      precision(128) {
}

bool NodeEditor::draw() {
    bool should_update_surface = false;

    ImNodes::PushColorStyle(ImNodesCol_Link, link_color);
    ImGui::Begin("Node Editor");

    if(ImGui::Button("Add Node")) {
        add_node_start_attribute = -1;
        add_node_position = ImGui::GetMousePos();
        ImGui::OpenPopup("##AddNodeMenu");
    }

    ImGui::SameLine(0.0f, 10.0f);

    ImGui::PushItemWidth(100.0f);
    ImGui::InputInt("precision", &precision);
    ImGui::PopItemWidth();

    ImNodes::BeginNodeEditor();

    for(auto& node : nodes) {
        if(node->draw()) {
            update_surface(node.get());
            should_update_surface = true;
        }
    }

    for(const auto& [attribute_id, attribute] : attributes) {
        if(attribute->io_type == IOType::IN && attribute->link_id != -1) {
            ImU32 color = attribute->color;
            ImNodes::PushColorStyle(ImNodesCol_Link, color);
            ImNodes::PushColorStyle(ImNodesCol_LinkHovered, IM_COL_DARKEN(color, 0.8f));
            ImNodes::PushColorStyle(ImNodesCol_LinkSelected, IM_COL_DARKEN(color, 0.9f));
            ImNodes::Link(attribute->link_id, attribute_id, attribute->linked_attribute->id);
            ImNodes::PopColorStyle();
            ImNodes::PopColorStyle();
            ImNodes::PopColorStyle();
        }
    }

    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_TopRight);
    ImNodes::EndNodeEditor();
    ImNodes::PopColorStyle();

    if(int start, end; ImNodes::IsLinkCreated(&start, &end)) {
        add_link(start, end);
    }

    if(int link_id; ImNodes::IsLinkDestroyed(&link_id)) {
        remove_link_by_id(link_id);
    }

    if(int start; ImNodes::IsLinkStarted(&start)) {
        link_color = attributes.at(start)->color;
    }

    if(int start; ImNodes::IsLinkDropped(&start)) {
        add_node_start_attribute = start;
        add_node_position = ImGui::GetMousePos();
        ImGui::OpenPopup("##AddNodeMenu");
    }

    if(int node_id; ImNodes::IsNodeHovered(&node_id) && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        right_click_node_id = node_id;
        ImGui::OpenPopup("##NodeRightClickMenu");
    }

    if(ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && !ImGui::GetIO().WantTextInput &&
       ImGui::IsKeyPressed(ImGuiKey_Delete, false)) {
        delete_selection();
    }

    draw_add_node_menu();
    draw_node_right_click_menu();

    ImGui::End();

    return should_update_surface;
}

void NodeEditor::update_surface(const NENode* node) {
    Blob* blob = node->get_blob();

    if(blob != nullptr) {
        surface.root = blob;
        ImGui::FocusWindow(ImGui::FindWindowByName("Main"));
    }
}

int NodeEditor::add_link(int start, int end) {
    NEAttribute& start_attr = *attributes.at(start);
    NEAttribute& end_attr = *attributes.at(end);

    if(start_attr.io_type != end_attr.io_type) {
        if(start_attr.io_type == IOType::IN) {
            remove_link_by_attr(start_attr);
        }
        if(end_attr.io_type == IOType::IN) {
            remove_link_by_attr(end_attr);
        }
        start_attr.link_id = end_attr.link_id = next_link_id;
        start_attr.linked_attribute = &end_attr;
        end_attr.linked_attribute = &start_attr;
        return next_link_id++;
    }

    return -1;
}

void NodeEditor::remove_link_by_attr(NEAttribute& attribute) {
    if(attribute.linked_attribute) {
        attribute.linked_attribute->link_id = -1;
        attribute.linked_attribute->linked_attribute = nullptr;
    }
    attribute.link_id = -1;
    attribute.linked_attribute = nullptr;
}

void NodeEditor::remove_link_by_id(int link_id) {
    for(auto& [id, attribute] : attributes) {
        if(attribute->link_id == link_id) {
            remove_link_by_attr(*attribute);
            return;
        }
    }
}

void NodeEditor::delete_node(int node_id) {
    auto ite = std::find_if(nodes.begin(), nodes.end(), [node_id](const std::unique_ptr<NENode>& node) {
        return node->id == node_id;
    });

    if(ite != nodes.end()) {
        std::vector<NEAttribute*> node_attributes;
        (*ite)->get_attributes(node_attributes);
        for(NEAttribute* attribute : node_attributes) {
            remove_link_by_attr(*attribute);
            attributes.erase(attribute->id);
        }

        nodes.erase(ite);
    }
}

void NodeEditor::delete_selection() {
    const int num_links = ImNodes::NumSelectedLinks();
    if(num_links > 0) {
        std::vector<int> selected_links(num_links);
        ImNodes::GetSelectedLinks(selected_links.data());
        for(int link_id : selected_links) {
            remove_link_by_id(link_id);
        }
    }

    const int num_nodes = ImNodes::NumSelectedNodes();
    if(num_nodes > 0) {
        std::vector<int> selected_nodes(num_nodes);
        ImNodes::GetSelectedNodes(selected_nodes.data());
        for(int node_id : selected_nodes) {
            delete_node(node_id);
        }
    }

    ImNodes::ClearLinkSelection();
    ImNodes::ClearNodeSelection();
}

void NodeEditor::compute_mesh(Mesh& mesh, mat4& aabb_model, mat4& tight_aabb_model, float fov, vec3& pos) {
    std::size_t blobs_count = 0;
    AABB aabb = surface.root->get_aabb_and_blob_count(blobs_count);
    AABB tight_aabb = surface.compute_tight_aabb(aabb, 32);
    std::cout << "\nThe scene contains " << blobs_count << " blobs.\n";

    mesh.clear();
    surface.compute_mesh(mesh, precision, aabb);
    aabb_model = aabb.get_global_model_matrix();
    tight_aabb_model = tight_aabb.get_global_model_matrix();

    const vec3 center = (aabb.pmin + aabb.pmax) * 0.5f;
    const float half_w = (aabb.pmax.x - aabb.pmin.x) * 0.5f;
    const float half_h = (aabb.pmax.y - aabb.pmin.y) * 0.5f;

    const float tan_half_fov = std::tan(fov * 0.5f);

    const float dist_h = half_h / tan_half_fov;
    const float dist_w = half_w / (tan_half_fov * Window::get_aspect_ratio());
    const float dist = std::max(dist_h, dist_w);

    pos = vec3(center.x, center.y, aabb.pmin.z - dist);
}

void NodeEditor::draw_add_node_menu() {
    if(ImGui::BeginPopup("##AddNodeMenu")) {
        if(add_node_start_attribute < 0) {
            if(ImGui::MenuItem("Sphere")) {
                move_and_snap_node_to_grid(add_node<NENodeSphereBlob>(1.0f, vec3(0.0f), 1.0f).id, add_node_position);
            }

            if(ImGui::MenuItem("Capsule")) {
                move_and_snap_node_to_grid(
                    add_node<NENodeCapsuleBlob>(1.0f, vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), 1.0f).id,
                    add_node_position);
            }

            if(ImGui::MenuItem("Operation")) {
                move_and_snap_node_to_grid(add_node<NENodeOperationBlob>(1.0f, BlobOpType::BLEND).id,
                                           add_node_position);
            }

            if(ImGui::MenuItem("Transform")) {
                move_and_snap_node_to_grid(add_node<NENodeTransformBlob>(1.0f, BlobTransformType::translation).id,
                                           add_node_position);
            }

        } else {
            if(attributes.at(add_node_start_attribute)->io_type == IOType::IN && ImGui::MenuItem("Sphere")) {
                const auto& node = add_node<NENodeSphereBlob>(1.0f, vec3(0.0f), 1.0f);
                move_and_snap_node_to_grid(node.id, add_node_position);
                add_link(add_node_start_attribute, node.attr_blob.id);
            }

            if(attributes.at(add_node_start_attribute)->io_type == IOType::IN && ImGui::MenuItem("Capsule")) {
                const auto& node =
                    add_node<NENodeCapsuleBlob>(1.0f, vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f), 1.0f);
                move_and_snap_node_to_grid(node.id, add_node_position);
                add_link(add_node_start_attribute, node.attr_blob.id);
            }

            if(ImGui::MenuItem("Operation")) {
                const auto& node = add_node<NENodeOperationBlob>(1.0f, BlobOpType::BLEND);
                move_and_snap_node_to_grid(node.id, add_node_position);

                if(attributes.at(add_node_start_attribute)->io_type == IOType::IN) {
                    add_link(add_node_start_attribute, node.attr_blob.id);
                } else if(attributes.at(add_node_start_attribute)->io_type == IOType::OUT) {
                    add_link(add_node_start_attribute, node.attr_blob_left.id);
                }
            }

            if(ImGui::MenuItem("Transform")) {
                const auto& node = add_node<NENodeTransformBlob>(1.0f, BlobTransformType::translation);
                move_and_snap_node_to_grid(node.id, add_node_position);

                if(attributes.at(add_node_start_attribute)->io_type == IOType::IN) {
                    add_link(add_node_start_attribute, node.attr_blob.id);
                } else if(attributes.at(add_node_start_attribute)->io_type == IOType::OUT) {
                    add_link(add_node_start_attribute, node.attr_blob_child.id);
                }
            }
        }

        ImGui::EndPopup();
    }
}

void NodeEditor::draw_node_right_click_menu() {
    if(ImGui::BeginPopup("##NodeRightClickMenu")) {

        if(ImGui::MenuItem("Delete")) {
            delete_node(right_click_node_id);
            right_click_node_id = -1;
        }

        ImGui::EndPopup();
    }
}
