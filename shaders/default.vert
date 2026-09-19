/***************************************************************************************************
 * @file  default.vert
 * @brief Default vertex shader
 **************************************************************************************************/

#version 460 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

out vec3 v_normal;

uniform mat4 u_mvp;

void main() {
    vec4 pos = vec4(a_position, 1.0f);

    gl_Position = u_mvp * pos;
    v_normal = normalize(a_normal);
}
