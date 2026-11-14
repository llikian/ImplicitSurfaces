/***************************************************************************************************
 * @file  NEAttribute.hpp
 * @brief Declaration of the NEAttribute class
 **************************************************************************************************/

#pragma once

#include <string>
#include "maths/vec3.hpp"
#include "imgui.h"
#include "imnodes.h"

struct NENode; // Forward Declaration

// should be used with coeff in [0 ; 1]
#define IM_COL_DARKEN(col, coeff)                                                                                      \
    IM_COL32((ImU32) (coeff * (col >> IM_COL32_R_SHIFT & 0xff)),                                                       \
             (ImU32) (coeff * (col >> IM_COL32_G_SHIFT & 0xff)),                                                       \
             (ImU32) (coeff * (col >> IM_COL32_B_SHIFT & 0xff)),                                                       \
             col >> IM_COL32_A_SHIFT & 0xff)

// colors are inverted : alpha, blue, green then red
#define IM_COL_WHITE   IM_COL32(255, 255, 255, 255)
#define IM_COL_BLACK   IM_COL32(0, 0, 0, 255)
#define IM_COL_RED     IM_COL32(255, 0, 0, 255)
#define IM_COL_GREEN   IM_COL32(0, 255, 0, 255)
#define IM_COL_BLUE    IM_COL32(0, 0, 255, 255)
#define IM_COL_MAGENTA IM_COL32(255, 0, 255, 255)
#define IM_COL_YELLOW  IM_COL32(255, 255, 0, 255)
#define IM_COL_CYAN    IM_COL32(0, 255, 255, 255)

enum class IOType : bool { IN, OUT };

struct NEAttribute {
    NEAttribute(int id,
                const NENode* node,
                IOType io_type,
                const std::string& name,
                ImU32 color = IM_COL_WHITE,
                ImNodesPinShape pin_shape = ImNodesPinShape_CircleFilled);

    void draw_pre();
    void draw_post();

    int id;
    const NENode* node;
    IOType io_type;
    std::string name;
    ImU32 color;
    ImNodesPinShape pin_shape;

    int link_id;
    NEAttribute* linked_attribute;
};
