/***************************************************************************************************
 * @file  default.frag
 * @brief Default fragment shader
 **************************************************************************************************/

#version 460 core

in vec3 v_position;
in vec3 v_normal;

out vec4 frag_color;

uniform vec3 u_color;
uniform vec3 u_pmin;
uniform vec3 u_pmax;
uniform float u_ambient;
uniform float u_alpha;
uniform vec3 u_camera_front;

// const vec3 light_direction = normalize(vec3(0.0f, 1.0f, 1.0f));

void main() {
    vec3 light_direction = normalize(-u_camera_front);
    float cos_theta = max(0.0f, dot(normalize(v_normal), light_direction));
    float ambient = (cos_theta * (1.0f - u_ambient) + u_ambient);

    // vec3 color = u_color * ambient;

    vec3 diagonal = u_pmax - u_pmin;
    vec3 color = (v_position / diagonal) * 0.5f + 0.5f;

    frag_color = vec4(color * ambient, u_alpha);
}
