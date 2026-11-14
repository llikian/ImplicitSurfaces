/***************************************************************************************************
 * @file  Application.cpp
 * @brief Implementation of the Application class
 **************************************************************************************************/

#include "applications/Application.hpp"

#include "assets/Shader.hpp"
#include "blobs/Blob.hpp"
#include "blobs/OperationBlob.hpp"
#include "engine/EventHandler.hpp"
#include "engine/Window.hpp"
#include "glad/glad.h"
#include "maths/constants.hpp"
#include "mesh/primitives.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "imnodes.h"
#include "Surface.hpp"

Application::Application()
    : camera(vec3(0.0f, 0.0f, 3.0f), PI_HALF_F, 0.1f, 1024.0f),
      framebuffer(Window::get_width(), Window::get_height()),
      shader({ "shaders/default.vert", "shaders/default.frag" }, "Default"),
      line_shader({ "shaders/line_mesh.vert", "shaders/line_mesh.frag" }, "Line Mesh"),
      show_surface_aabb(false),
      show_surface_tight_aabb(true) {
    /* ---- Event Handler ---- */
    EventHandler::set_active_camera(&camera);
    EventHandler::associate_action_to_key(GLFW_KEY_1, false, []() {
        ImGui::FocusWindow(ImGui::FindWindowByName("Main"));
        EventHandler::get().hide_cursor();
    });
    EventHandler::associate_action_to_key(GLFW_KEY_2, false, []() {
        ImGui::FocusWindow(ImGui::FindWindowByName("Node Editor"));
        EventHandler::get().show_cursor();
    });

    /* ---- ImGui ---- */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui::GetIO().IniFilename = "data/imgui.ini";
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui_ImplGlfw_InitForOpenGL(Window::get_glfw(), true);
    ImGui_ImplOpenGL3_Init();

    ImNodes::CreateContext();

    /* ---- Node Editor ---- */
    ImNodesStyle& style = ImNodes::GetStyle();
    style.Flags |= ImNodesStyleFlags_GridLinesPrimary | ImNodesStyleFlags_GridSnapping;
    ImNodes::GetIO().LinkDetachWithModifierClick.Modifier = &ImGui::GetIO().KeyCtrl;

    NodeEditor& ne = node_editor;

    const auto& sphere0 = ne.add_node<NENodeSphereBlob>(1.0f, vec3(-0.5f, 0.0f, 0.0f), 1.0f);
    const auto& sphere1 = ne.add_node<NENodeSphereBlob>(1.0f, vec3(0.5f, 0.0f, 0.0f), 1.0f);
    move_and_snap_node_to_grid(sphere1.id, ImVec2(0.0f, 250.0f));
    const auto& blend_spheres = ne.add_node<NENodeOperationBlob>(1.0f, BlobOpType::BLEND);
    move_and_snap_node_to_grid(blend_spheres.id, ImVec2(400.0f, 0.0f));

    ne.add_link(sphere0.attr_blob.id, blend_spheres.attr_blob_left.id);
    ne.add_link(sphere1.attr_blob.id, blend_spheres.attr_blob_right.id);

    node_editor.update_surface(&blend_spheres);
    update_surface_mesh();

    /* ---- Other ---- */
    // glfwSwapInterval(0); // disable vsync
    create_wireframe_cube_mesh(wireframe_cube);
    create_wireframe_sphere_mesh(wireframe_sphere, 32);
}

Application::~Application() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Application::run() {
    /* Main Loop */
    while(!Window::should_close()) {
        EventHandler::poll_and_handle_events();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        handle_dockspace();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        vp_matrix = camera.get_view_projection_matrix();

        bool is_main_window_drawn = draw_imgui_main_window();
        if(node_editor.draw()) {
            update_surface_mesh();
        }

        if(is_main_window_drawn) {
            ImGui::Begin("Debug");
            ImGui::Checkbox("Show crude AABB", &show_surface_aabb);
            ImGui::Checkbox("Show tight AABB", &show_surface_tight_aabb);
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        Window::swap_buffers();
    }
}

void Application::update_surface_mesh() {
    vec3 pos;
    node_editor.compute_mesh(surface_mesh, surface_aabb_model, surface_tight_aabb_model, camera.get_fov(), pos);
    camera.set_position(pos);
    camera.look_at_point(pos + vec3(0.0f, 0.0f, 1.0f));
}

void Application::draw() {
    framebuffer.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    shader.set_uniform("u_ambient", 0.3f);
    shader.set_uniform("u_alpha", 1.0f);
    shader.set_uniform("u_camera_front", camera.get_direction());
    shader.set_uniform("u_mvp", vp_matrix);
    surface_mesh.draw();

    line_shader.use();
    glLineWidth(3.0f);
    if(show_surface_aabb) {
        line_shader.set_uniform("u_mvp", vp_matrix * surface_aabb_model);
        line_shader.set_uniform("u_color", vec3(1.0f, 1.0f, 0.0f));
        wireframe_cube.draw();
    }
    if(show_surface_tight_aabb) {
        line_shader.set_uniform("u_mvp", vp_matrix * surface_tight_aabb_model);
        line_shader.set_uniform("u_color", vec3(1.0f, 1.0f, 1.0f));
        wireframe_cube.draw();
    }
    glLineWidth(1.0f);

    Framebuffer::bind_default();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
};

bool Application::draw_imgui_main_window() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    bool is_window_drawn = ImGui::Begin("Main", nullptr, ImGuiWindowFlags_NoTitleBar);
    ImGui::PopStyleVar(1);

    ImVec2 avail = ImGui::GetContentRegionAvail();
    avail.x = avail.x < 1.0f ? 1.0f : avail.x;
    avail.y = avail.y < 1.0f ? 1.0f : avail.y;

    vec2 res = framebuffer.get_resolution();
    if(static_cast<int>(avail.x) != res.x || static_cast<int>(avail.y) != res.y) {
        framebuffer.resize(avail.x, avail.y);
        glViewport(0, 0, avail.x, avail.y);
        camera.update_projection_matrix(avail.x, avail.y);
    }

    draw();

    ImGui::Image(framebuffer.get_texture_id(), avail, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

    ImGui::End();

    return is_window_drawn;
}

void Application::handle_dockspace() const {
    ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(0, nullptr);

    static bool init_dock = !std::filesystem::exists("data/imgui.ini");
    if(init_dock) {
        ImGuiID id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);

        ImGui::DockBuilderSetNodeSize(dockspace_id, ImGui::GetMainViewport()->Size);

        ImGui::DockBuilderDockWindow("Main", dockspace_id);
        ImGui::DockBuilderDockWindow("Node Editor", dockspace_id);

        ImGui::DockBuilderDockWindow("Debug", id_left);

        ImGui::DockBuilderFinish(dockspace_id);

        init_dock = false;
    }
}
