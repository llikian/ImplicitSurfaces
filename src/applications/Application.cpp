/***************************************************************************************************
 * @file  Application.cpp
 * @brief Implementation of the Application class
 **************************************************************************************************/

#include "applications/Application.hpp"

#include "assets/Shader.hpp"
#include "engine/EventHandler.hpp"
#include "engine/Window.hpp"
#include "glad/glad.h"
#include "maths/constants.hpp"
#include "maths/functions.hpp"
#include "maths/geometry.hpp"
#include "mesh/primitives.hpp"
#include "utility/Random.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Surface.hpp"

Application::Application() : camera(vec3(0.0f, 0.0f, 3.0f), PI_HALF_F, 0.1f, 1024.0f) {
    /* ---- Event Handler ---- */
    EventHandler::set_active_camera(&camera);

    /* ---- ImGui ---- */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetIO().IniFilename = "data/imgui.ini";
    ImGui_ImplGlfw_InitForOpenGL(Window::get_glfw(), true);
    ImGui_ImplOpenGL3_Init();

    /* ---- Other ---- */
    // glfwSwapInterval(0); // disable vsync
}

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

Blob* add_recursive_spheres(int depth) {
    float bound_len = 7.0f;
    static const vec3 min_bound(-bound_len);
    static const vec3 max_bound(bound_len);
    static const float min_radius = 1.0f;
    static const float max_radius = 5.0f;

    if(depth == 0) {
        return new SphereBlob(Random::get_vec3(min_bound, max_bound), Random::get_float(min_radius, max_radius));
    }

    return new SumBlob(add_recursive_spheres(depth - 1), add_recursive_spheres(depth - 1));
}

void Application::run() {
    Surface surface;

    // SphereBlob A(vec3(0.0f, 0.0f, 0.0f), 3.0f);
    // SphereBlob B(vec3(0.0f, 0.0f, 1.5f), 2.0f);
    // CapsuleBlob C(vec3(0.0f, -2.0f, 0.0f), vec3(0.0f, 2.0f, 0.0f), 2.0f);

    // DifferenceBlob AdiffB(&A, &B);
    // DifferenceBlob root(&C, &AdiffB);

    // UnionBlob AuB(&A, &B);

    Blob* root = add_recursive_spheres(7);
    surface.root = root;
    Mesh surface_mesh = surface.compute_mesh(100);

    Shader shader({ "shaders/default.vert", "shaders/default.frag" }, "Default");
    Shader line_shader({ "shaders/line_mesh.vert", "shaders/line_mesh.frag" }, "Line Mesh");

    mat4 vp_matrix;

    /* Main Loop */
    while(!Window::should_close()) {
        EventHandler::poll_and_handle_events();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        vp_matrix = camera.get_view_projection_matrix();

        shader.use();
        shader.set_uniform("u_mvp", vp_matrix);
        shader.set_uniform("u_color", vec3(0.84, 0.37, 0.8));
        shader.set_uniform("u_ambient", 0.3f);
        shader.set_uniform("u_alpha", 1.0f);
        shader.set_uniform("u_camera_front", camera.get_direction());

        surface_mesh.draw();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        Window::swap_buffers();
    }
}
