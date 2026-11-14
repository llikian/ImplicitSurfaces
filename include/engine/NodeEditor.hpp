/***************************************************************************************************
 * @file  NodeEditor.hpp
 * @brief Declaration of the NodeEditor class
 **************************************************************************************************/

#pragma once

#include <memory>
#include <utility>
#include <vector>
#include "NENode.hpp"
#include "Surface.hpp"
#include "Window.hpp"

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

    void compute_mesh(Mesh& mesh, mat4& aabb_model, float fov, vec3& pos);

private:
    void draw_add_node_menu();
    void draw_node_right_click_menu();

    std::vector<std::unique_ptr<NENode>> nodes;
    std::unordered_map<int, NEAttribute*> attributes;

    int next_node_index;
    int next_attribute_index;
    int next_link_id;

    ImU32 link_color;

    int add_node_start_attribute;
    ImVec2 add_node_position;

    int right_click_node_id;

    Surface surface;
    int precision;
};
