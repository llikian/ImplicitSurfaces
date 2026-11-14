/***************************************************************************************************
 * @file  NEAttribute.cpp
 * @brief Implementation of the NEAttribute class
 **************************************************************************************************/

#include "engine/NEAttribute.hpp"

#include "imnodes.h"

NEAttribute::NEAttribute(int id,
                         const NENode* node,
                         IOType io_type,
                         const std::string& name,
                         int color,
                         ImNodesPinShape pin_shape)
    : id(id),
      node(node),
      io_type(io_type),
      name(name),
      color(color),
      pin_shape(pin_shape),
      link_id(-1),
      linked_attribute(nullptr) {
}

void NEAttribute::draw_pre() {
    ImNodes::PushColorStyle(ImNodesCol_Pin, color);
    ImNodes::PushColorStyle(ImNodesCol_PinHovered, IM_COL_DARKEN(color, 0.8f));
    switch(io_type) {
        case IOType::IN:  ImNodes::BeginInputAttribute(id, pin_shape); break;
        case IOType::OUT: ImNodes::BeginOutputAttribute(id, pin_shape); break;
    }
}

void NEAttribute::draw_post() {
    switch(io_type) {
        case IOType::IN:  ImNodes::EndInputAttribute(); break;
        case IOType::OUT: ImNodes::EndOutputAttribute(); break;
    }
    ImNodes::PopColorStyle();
    ImNodes::PopColorStyle();
}
