/***************************************************************************************************
 * @file  Application.hpp
 * @brief Declaration of the Application class
 **************************************************************************************************/

#pragma once

#include "assets/Camera.hpp"
#include "assets/Shader.hpp"
#include "engine/Framebuffer.hpp"
#include "engine/NodeEditor.hpp"
#include "mesh/Mesh.hpp"
#include "Surface.hpp"

/**
 * @class Application
 * @brief Core of the project. Assembles everything together and handles the main loop.
 */
class Application {
public:
    /**
     * @brief Constructor.
     */
    Application();

    /**
     * @brief Frees all resources.
     */
    ~Application();

    /**
     * @brief Executes the main loop of the application.
     */
    void run();

    void update_surface_mesh();

private:
    void draw_mesh(const Mesh& mesh, const mat4& model, const mat4& aabb_model);

    void draw();
    void draw_imgui_main_window();

    void handle_dockspace() const;

    Camera camera; ///< The camera.

    Framebuffer framebuffer; ///< The framebuffer used to render.

    Shader shader;
    Shader line_shader;

    Mesh wireframe_cube;
    Mesh wireframe_sphere;

    mat4 vp_matrix;
    inline static mat4 IDENTITY { 1.0f };

    NodeEditor node_editor;
    Mesh surface_mesh;
    mat4 surface_aabb_model;
};
