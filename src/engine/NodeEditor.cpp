/***************************************************************************************************
 * @file  NodeEditor.cpp
 * @brief Implementation of the NodeEditor class
 **************************************************************************************************/

#include "engine/NodeEditor.hpp"

#include <fstream>
#include "blobs/BezierBlob.hpp"
#include "engine/Window.hpp"
#include "imgui_stdlib.h"
#include "imnodes.h"

NodeEditor::NodeEditor()
    : next_node_index(0),
      next_attribute_index(0),
      next_link_id(0),
      last_root_node_id(-1),
      link_color(IM_COL32_WHITE),
      add_node_start_attribute(-1),
      right_click_node_id(-1),
      precision(128) {
}

bool NodeEditor::draw() {
    bool should_update_surface = false;

    ImNodes::PushColorStyle(ImNodesCol_Link, link_color);
    ImGui::Begin("Node Editor");

    ImGui::SameLine();
    if(ImGui::Button("Load")) {
        ImGui::OpenPopup("##LoadFile");
    }

    ImGui::SameLine();
    if(ImGui::Button("Save")) {
        ImGui::OpenPopup("##SaveFile");
    }

    ImGui::SameLine();
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
    draw_save_file_modal();
    draw_load_file_modal();

    ImGui::End();

    return should_update_surface;
}

void NodeEditor::update_surface(const NENode* node) {
    Blob* blob = node->get_blob();

    if(blob != nullptr) {
        surface.root = blob;
        last_root_node_id = node->id;
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

    if(last_root_node_id == node_id) {
        last_root_node_id = -1;
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
    surface.root->precompute(blobs_count);
    std::cout << "\nThe scene contains " << blobs_count << " blobs.\n";

    AABB aabb = surface.root->aabb;
    AABB tight_aabb = surface.compute_tight_aabb(aabb, 32);

    mesh.clear();
    surface.compute_mesh(mesh, precision, tight_aabb);
    aabb_model = aabb.get_global_model_matrix();
    tight_aabb_model = tight_aabb.get_global_model_matrix();

    vec3 center = (tight_aabb.pmin + tight_aabb.pmax) * 0.5f;
    float half_w = (tight_aabb.pmax.x - tight_aabb.pmin.x) * 0.5f;
    float half_h = (tight_aabb.pmax.y - tight_aabb.pmin.y) * 0.5f;

    float tan_half_fov = std::tan(fov * 0.5f);

    float dist_h = half_h / tan_half_fov;
    float dist_w = half_w / (tan_half_fov * Window::get_aspect_ratio());
    float dist = std::max(dist_h, dist_w);

    pos = vec3(center.x, center.y, tight_aabb.pmin.z - dist);
}

void NodeEditor::save_to_file(const std::filesystem::path& path) {
    std::ofstream file(path);
    if(!file.is_open()) {
        throw std::runtime_error(std::string("Couldn't open file: ") + path.string());
    }

    file << std::setprecision(9);

    static constexpr std::string TAB = "  ";

    file << last_root_node_id << '\n';
    file << nodes.size() << '\n';
    for(const auto& node : nodes) {
        NENodeType node_type = node->get_node_type();
        file << node->id << '\n';
        ImVec2 pos = ImNodes::GetNodeEditorSpacePos(node->id);
        file << TAB << pos.x << ' ' << pos.y << '\n';
        file << TAB << node->title << '\n';
        file << TAB << static_cast<int>(node_type) << '\n';
        file << TAB << node->title_bar_color << '\n';

        switch(node_type) {
            case NENodeType::sphere: {
                const auto* tnode = static_cast<const NENodeSphereBlob*>(node.get());
                const auto* blob = static_cast<const SphereBlob*>(tnode->blob.get());

                file << TAB << TAB << tnode->attr_blob.id << '\n';

                file << TAB << TAB << blob->scale << '\n';
                file << TAB << TAB << blob->color.x << ' ' << blob->color.y << ' ' << blob->color.z << '\n';
                file << TAB << TAB << blob->center.x << ' ' << blob->center.y << ' ' << blob->center.z << '\n';
                file << TAB << TAB << blob->radius << '\n';

                break;
            }
            case NENodeType::capsule: {
                const auto* tnode = static_cast<const NENodeCapsuleBlob*>(node.get());
                const auto* blob = static_cast<const CapsuleBlob*>(tnode->blob.get());

                file << TAB << TAB << tnode->attr_blob.id << '\n';

                file << TAB << TAB << blob->scale << '\n';
                file << TAB << TAB << blob->color.x << ' ' << blob->color.y << ' ' << blob->color.z << '\n';
                file << TAB << TAB << blob->A.x << ' ' << blob->A.y << ' ' << blob->A.z << '\n';
                file << TAB << TAB << blob->B.x << ' ' << blob->B.y << ' ' << blob->B.z << '\n';
                file << TAB << TAB << blob->radius << '\n';

                break;
            }
            case NENodeType::operation: {
                const auto* tnode = static_cast<const NENodeOperationBlob*>(node.get());
                const auto* blob = static_cast<const OperationBlob*>(tnode->blob.get());

                file << TAB << TAB << tnode->attr_blob.id << '\n';
                file << TAB << TAB << tnode->attr_blob_left.id << '\n';
                file << TAB << TAB << tnode->attr_blob_right.id << '\n';

                file << TAB << TAB << static_cast<int>(tnode->op_type) << '\n';

                file << TAB << TAB << blob->scale << '\n';

                break;
            }
            case NENodeType::transform: {
                const auto* tnode = static_cast<const NENodeTransformBlob*>(node.get());
                const auto* blob = static_cast<const TransformBlob*>(tnode->blob.get());

                file << TAB << TAB << tnode->attr_blob.id << '\n';
                file << TAB << TAB << tnode->attr_blob_child.id << '\n';

                file << TAB << TAB << static_cast<int>(tnode->transform_type) << '\n';

                file << TAB << TAB << blob->scale << '\n';

                switch(tnode->transform_type) {
                    case BlobTransformType::translation: {
                        vec3 t = static_cast<const TranslationBlob*>(blob)->translation;
                        file << TAB << TAB << t.x << ' ' << t.y << ' ' << t.z << '\n';
                        break;
                    }
                    case BlobTransformType::rotation: {
                        const auto* tblob = static_cast<const RotationBlob*>(blob);
                        file << TAB << TAB << tblob->axis.x << ' ' << tblob->axis.y << ' ' << tblob->axis.z << '\n';
                        file << TAB << TAB << tblob->angle << '\n';
                        break;
                    }
                    case BlobTransformType::twist:
                        file << TAB << TAB << static_cast<const TwistBlob*>(blob)->rate << '\n';
                        break;
                    case BlobTransformType::taper:
                        file << TAB << TAB << static_cast<const TaperBlob*>(blob)->rate << '\n';
                        break;
                }

                break;
            }
            case NENodeType::bezier: {
                const auto* tnode = static_cast<const NENodeBezierBlob*>(node.get());
                const auto* blob = static_cast<const BezierBlob*>(tnode->blob.get());

                file << TAB << TAB << tnode->attr_blob.id << '\n';

                file << TAB << TAB << blob->scale << '\n';
                file << TAB << TAB << blob->spheres_count << '\n';
                file << TAB << TAB << blob->p0.x << ' ' << blob->p0.y << ' ' << blob->p0.z << '\n';
                file << TAB << TAB << blob->p1.x << ' ' << blob->p1.y << ' ' << blob->p1.z << '\n';
                file << TAB << TAB << blob->p2.x << ' ' << blob->p2.y << ' ' << blob->p2.z << '\n';
                file << TAB << TAB << blob->p3.x << ' ' << blob->p3.y << ' ' << blob->p3.z << '\n';
                file << TAB << TAB << blob->radius_start << '\n';
                file << TAB << TAB << blob->radius_end << '\n';
                file << TAB << TAB << blob->color_start.x << ' ' << blob->color_start.y << ' ' << blob->color_start.z
                     << '\n';
                file << TAB << TAB << blob->color_end.x << ' ' << blob->color_end.y << ' ' << blob->color_end.z << '\n';

                break;
            }
        }
    }

    file << attributes.size() << '\n';
    for(const auto& [_, attr] : attributes) {
        file << attr->id << '\n';
        file << TAB << attr->node->id << '\n';
        file << TAB << static_cast<int>(attr->io_type) << '\n';
        file << TAB << attr->name << '\n';
        file << TAB << attr->color << '\n';
        file << TAB << attr->pin_shape << '\n';
        file << TAB << attr->link_id << '\n';
        if(attr->linked_attribute != nullptr) {
            file << TAB << attr->linked_attribute->id << '\n';
        } else {
            file << TAB << -1 << '\n';
        }
    }

    std::cout << "Saved node editor to file " << path << " successfully\n";
}

void NodeEditor::load_from_file(const std::filesystem::path& path) {
    constexpr auto load_error = [](const std::filesystem::path& path, const char* what) -> void {
        throw std::runtime_error(std::string("Invalid node editor file (") + what + "): " + path.string());
    };

    std::ifstream file(path);
    if(!file.is_open()) {
        throw std::runtime_error(std::string("Couldn't open file: ") + path.string());
    }

    static constexpr std::string TAB = "  ";

    NodeEditor loaded;
    std::unordered_map<int, NEAttribute*> old_attr_id_to_new_attr;
    std::vector<std::pair<int, ImVec2>> positions;

    if(!(file >> loaded.last_root_node_id)) {
        load_error(path, "last root node id");
    }
    NENode* root = nullptr;

    std::size_t node_count = 0;
    if(!(file >> node_count)) {
        load_error(path, "node count");
    }

    for(std::size_t i = 0; i < node_count; ++i) {
        int saved_node_id;
        ImVec2 pos;
        std::string title;
        int node_type_int;
        ImU32 title_bar_color;

        file >> saved_node_id >> pos.x >> pos.y;
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::getline(file, title);
        if(title.starts_with(TAB)) {
            title.erase(0, TAB.size());
        }
        file >> node_type_int >> title_bar_color;
        if(!file) {
            load_error(path, "node header");
        }

        NENode* node = nullptr;

        switch(static_cast<NENodeType>(node_type_int)) {
            case NENodeType::sphere: {
                int attr_blob_id;
                float scale, radius;
                vec3 color, center;
                file >> attr_blob_id >> scale >> color >> center >> radius;
                if(!file) {
                    load_error(path, "sphere node");
                }

                loaded.add_node<NENodeSphereBlob>(scale, center, radius);
                auto* tnode = static_cast<NENodeSphereBlob*>(loaded.nodes.back().get());
                static_cast<SphereBlob*>(tnode->get_blob())->color = color;

                old_attr_id_to_new_attr[attr_blob_id] = &tnode->attr_blob;
                node = tnode;
                break;
            }
            case NENodeType::capsule: {
                int attr_blob_id;
                float scale, radius;
                vec3 color, A, B;
                file >> attr_blob_id >> scale >> color >> A >> B >> radius;
                if(!file) {
                    load_error(path, "capsule node");
                }

                loaded.add_node<NENodeCapsuleBlob>(scale, A, B, radius);
                auto* tnode = static_cast<NENodeCapsuleBlob*>(loaded.nodes.back().get());
                static_cast<CapsuleBlob*>(tnode->get_blob())->color = color;

                old_attr_id_to_new_attr[attr_blob_id] = &tnode->attr_blob;
                node = tnode;
                break;
            }
            case NENodeType::operation: {
                int attr_blob_id, attr_left_id, attr_right_id, op_type_int;
                float scale;
                file >> attr_blob_id >> attr_left_id >> attr_right_id >> op_type_int >> scale;
                if(!file) {
                    load_error(path, "operation node");
                }

                loaded.add_node<NENodeOperationBlob>(scale, static_cast<BlobOpType>(op_type_int));
                auto* tnode = static_cast<NENodeOperationBlob*>(loaded.nodes.back().get());

                old_attr_id_to_new_attr[attr_blob_id] = &tnode->attr_blob;
                old_attr_id_to_new_attr[attr_left_id] = &tnode->attr_blob_left;
                old_attr_id_to_new_attr[attr_right_id] = &tnode->attr_blob_right;
                node = tnode;
                break;
            }
            case NENodeType::transform: {
                int attr_blob_id, attr_child_id, transform_type_int;
                float scale;
                file >> attr_blob_id >> attr_child_id >> transform_type_int >> scale;
                if(!file) {
                    load_error(path, "transform node");
                }

                const auto transform_type = static_cast<BlobTransformType>(transform_type_int);
                loaded.add_node<NENodeTransformBlob>(scale, transform_type);
                auto* tnode = static_cast<NENodeTransformBlob*>(loaded.nodes.back().get());

                switch(transform_type) {
                    case BlobTransformType::translation: {
                        vec3 translation;
                        file >> translation;
                        static_cast<TranslationBlob*>(tnode->blob.get())->translation = translation;
                        break;
                    }
                    case BlobTransformType::rotation: {
                        vec3 axis;
                        float angle;
                        file >> axis >> angle;
                        auto* blob = static_cast<RotationBlob*>(tnode->blob.get());
                        blob->axis = axis;
                        blob->angle = angle;
                        break;
                    }
                    case BlobTransformType::twist: {
                        float rate;
                        file >> rate;
                        static_cast<TwistBlob*>(tnode->blob.get())->rate = rate;
                        break;
                    }
                    case BlobTransformType::taper:
                        float rate;
                        file >> rate;
                        static_cast<TaperBlob*>(tnode->blob.get())->rate = rate;
                        break;
                }
                if(!file) {
                    load_error(path, "transform parameters");
                }

                old_attr_id_to_new_attr[attr_blob_id] = &tnode->attr_blob;
                old_attr_id_to_new_attr[attr_child_id] = &tnode->attr_blob_child;
                node = tnode;
                break;
            }
            case NENodeType::bezier: {
                int attr_blob_id, spheres_count;
                float scale, radius_start, radius_end;
                vec3 p0, p1, p2, p3;
                vec3 color_start, color_end;
                file >> attr_blob_id >> scale >> spheres_count >> p0 >> p1 >> p2 >> p3 >> radius_start >> radius_end >>
                    color_start >> color_end;

                if(!file) {
                    load_error(path, "bezier node");
                }

                loaded.add_node<NENodeBezierBlob>(scale);
                auto* tnode = static_cast<NENodeBezierBlob*>(loaded.nodes.back().get());
                auto* blob = static_cast<BezierBlob*>(tnode->get_blob());
                blob->spheres_count = spheres_count;
                blob->p0 = p0;
                blob->p1 = p1;
                blob->p2 = p2;
                blob->p3 = p3;
                blob->radius_start = radius_start;
                blob->radius_end = radius_end;
                blob->color_start = color_start;
                blob->color_end = color_end;

                old_attr_id_to_new_attr[attr_blob_id] = &tnode->attr_blob;
                node = tnode;
                break;
            }
            default: load_error(path, "unknown node type");
        }

        node->title = title;
        node->title_bar_color = title_bar_color;

        if(loaded.last_root_node_id == saved_node_id) {
            loaded.last_root_node_id = node->id;
            root = node;
        }

        positions.emplace_back(node->id, pos);
    }

    std::size_t attribute_count = 0;
    if(!(file >> attribute_count)) {
        load_error(path, "attribute count");
    }

    std::vector<std::pair<int, int>> links;
    for(std::size_t i = 0; i < attribute_count; ++i) {
        int id, node_id, io_type, pin_shape, link_id, linked_id;
        ImU32 color;
        std::string name;
        file >> id >> node_id >> io_type >> name >> color >> pin_shape >> link_id >> linked_id;
        if(!file) {
            load_error(path, "attribute");
        }

        if(io_type == static_cast<int>(IOType::IN) && linked_id >= 0) {
            links.emplace_back(id, linked_id);
        }
    }

    for(const auto& [input_id, output_id] : links) {
        const auto input = old_attr_id_to_new_attr.find(input_id);
        const auto output = old_attr_id_to_new_attr.find(output_id);
        if(input == old_attr_id_to_new_attr.end() || output == old_attr_id_to_new_attr.end()) {
            load_error(path, "link to an unknown attribute");
        }
        if(loaded.add_link(input->second->id, output->second->id) < 0) {
            load_error(path, "invalid link");
        }
    }

    nodes.swap(loaded.nodes);
    attributes.swap(loaded.attributes);
    std::swap(next_node_index, loaded.next_node_index);
    std::swap(next_attribute_index, loaded.next_attribute_index);
    std::swap(next_link_id, loaded.next_link_id);
    std::swap(last_root_node_id, loaded.last_root_node_id);

    surface.root = nullptr;
    add_node_start_attribute = -1;
    right_click_node_id = -1;

    ImNodes::ClearNodeSelection();
    ImNodes::ClearLinkSelection();
    for(const auto& [node_id, pos] : positions) {
        move_and_snap_node_to_grid(node_id, pos);
    }

    if(root != nullptr) {
        update_surface(root);
    }
}

int NodeEditor::add_recursive_spheres(int level, const ImVec2& position, float offset) {
    if(level == 0) {
        auto& leaf = add_node<NENodeSphereBlob>(1.0f, Random::get_vec3(-10.0f, 10.0f), Random::get_float(1.0f, 5.0f));
        move_and_snap_node_to_grid(leaf.id, position);
        return leaf.attr_blob.id;
    }

    auto& node = add_node<NENodeOperationBlob>(1.0f, BlobOpType::BLEND);
    move_and_snap_node_to_grid(node.id, position);

    float x = position.x - 450.0f;
    add_link(add_recursive_spheres(level - 1, ImVec2(x, position.y - offset), offset * 0.5f), node.attr_blob_left.id);
    add_link(add_recursive_spheres(level - 1, ImVec2(x, position.y + offset), offset * 0.5f), node.attr_blob_right.id);
    return node.attr_blob.id;
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

            if(ImGui::MenuItem("Bezier")) {
                move_and_snap_node_to_grid(add_node<NENodeBezierBlob>(1.0f).id, add_node_position);
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

            if(attributes.at(add_node_start_attribute)->io_type == IOType::IN && ImGui::MenuItem("Bezier")) {
                const auto& node = add_node<NENodeBezierBlob>(1.0f);
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

void NodeEditor::draw_load_file_modal() {
    static constexpr std::string path = "data/";
    static constexpr std::string extension = ".ne";

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if(ImGui::BeginPopupModal("##LoadFile", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        for(const auto& entry : std::filesystem::directory_iterator(path)) {
            if(entry.is_regular_file() && entry.path().extension() == extension) {
                if(ImGui::Selectable(entry.path().filename().c_str())) {
                    load_from_file(entry.path());
                    ImGui::CloseCurrentPopup();
                }
            }
        }

        ImGui::Separator();
        if(ImGui::Button("Cancel") || (!ImGui::GetIO().WantTextInput && ImGui::IsKeyPressed(ImGuiKey_Escape))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}

void NodeEditor::draw_save_file_modal() {
    static constexpr std::string path = "data/";
    static constexpr std::string extension = ".ne";
    static std::string filename;

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if(ImGui::BeginPopupModal("##SaveFile", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("File Name", &filename);

        if(ImGui::Button("Save") && !filename.empty()) {
            save_to_file(path + filename + extension);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if(ImGui::Button("Cancel") || (!ImGui::GetIO().WantTextInput && ImGui::IsKeyPressed(ImGuiKey_Escape))) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
