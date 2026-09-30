/***************************************************************************************************
 * @file  default.frag
 * @brief Default fragment shader
 **************************************************************************************************/

#version 460 core

in vec3 v_position;
in vec3 v_normal;
in vec3 v_color;

out vec4 frag_color;

uniform vec3 u_color;
uniform float u_ambient;
uniform float u_alpha;
uniform vec3 u_camera_front;

// const vec3 light_direction = normalize(vec3(0.0f, 1.0f, 1.0f));

void main() {
    vec3 light_direction = normalize(-u_camera_front);
    float cos_theta = max(0.0f, dot(normalize(v_normal), light_direction));
    float ambient = (cos_theta * (1.0f - u_ambient) + u_ambient);

    frag_color = vec4(v_color * ambient, u_alpha);
}
