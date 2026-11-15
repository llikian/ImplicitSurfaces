/***************************************************************************************************
 * @file  NodeEditor.hpp
 * @brief Declaration of the NodeEditor class
 **************************************************************************************************/

#pragma once

#include <filesystem>
#include <memory>
#include <utility>
#include <vector>
#include "NENode.hpp"
#include "Surface.hpp"

inline void move_and_snap_node_to_grid(int node_id, const ImVec2& position) {
    ImNodes::SetNodeEditorSpacePos(node_id, position);
    ImNodes::SnapNodeToGrid(node_id);
}

class NodeEditor {
public:
    NodeEditor();

    bool draw();

    void update_surface(const NENode* node);

    int add_link(int start, int end);
    void remove_link_by_attr(NEAttribute& attribute);
    void remove_link_by_id(int link_id);

    template <typename T, typename... Args>
    const T& add_node(Args&&... args) {
        nodes.push_back(std::make_unique<T>(next_node_index, next_attribute_index, std::forward<Args>(args)...));

        std::vector<NEAttribute*> node_attributes;
        nodes.back()->get_attributes(node_attributes);
        for(NEAttribute* attribute : node_attributes) {
            attributes.emplace(attribute->id, attribute);
        }
        next_node_index++;

        return *static_cast<const T*>(nodes.back().get());
    }

    void delete_node(int node_id);

    void delete_selection();

    void compute_mesh(Mesh& mesh, mat4& aabb_model, mat4& aabb_tight_model, float fov, vec3& pos);

    void save_to_file(const std::filesystem::path& path = "data/scene.ne");
    void load_from_file(const std::filesystem::path& path = "data/scene.ne");

    // int add_recursive_spheres(int level);
    int add_recursive_spheres(int level, const ImVec2& position, float offset);

private:
    void draw_add_node_menu();
    void draw_node_right_click_menu();
    void draw_load_file_modal();
    void draw_save_file_modal();

    std::vector<std::unique_ptr<NENode>> nodes;
    std::unordered_map<int, NEAttribute*> attributes;

    int next_node_index;
    int next_attribute_index;
    int next_link_id;

    int last_root_node_id;

    ImU32 link_color;

    int add_node_start_attribute;
    ImVec2 add_node_position;

    int right_click_node_id;

    Surface surface;
    int precision;
};
