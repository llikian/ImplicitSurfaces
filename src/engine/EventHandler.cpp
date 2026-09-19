/***************************************************************************************************
 * @file  EventHandler.cpp
 * @brief Implementation of the EventHandler class
 **************************************************************************************************/

#include "engine/EventHandler.hpp"

#include "glad/glad.h" // Needs to go first to avoid error

#include "engine/callbacks.hpp"
#include "engine/Window.hpp"

EventHandler::EventHandler()
    : left_click_func(nullptr),
      time(glfwGetTime()), delta(0.0f),
      active_camera(nullptr),
      b_is_cursor_visible(glfwGetInputMode(Window::get_glfw(), GLFW_CURSOR) == GLFW_CURSOR_NORMAL),
      b_is_face_culling_enabled(true), b_is_wireframe_enabled(false) {
    /* ---- Callbacks ---- */
    glfwSetWindowSizeCallback(Window::get_glfw(), window_size_callback);
    glfwSetFramebufferSizeCallback(Window::get_glfw(), framebuffer_size_callback);
    glfwSetKeyCallback(Window::get_glfw(), key_callback);
    glfwSetCursorPosCallback(Window::get_glfw(), cursor_position_callback);
    glfwSetMouseButtonCallback(Window::get_glfw(), mouse_button_callback);

    /* ---- Key Actions ---- */
    /* General */
    do_associate_action_to_key(GLFW_KEY_ESCAPE, false, [] { glfwSetWindowShouldClose(Window::get_glfw(), true); });

    /* Toggles */
    do_associate_action_to_key(GLFW_KEY_TAB, false, [this] {
        int mode = b_is_cursor_visible ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
        glfwSetInputMode(Window::get_glfw(), GLFW_CURSOR, mode);
        b_is_cursor_visible = !b_is_cursor_visible;
    });
    do_associate_action_to_key(GLFW_KEY_F, false, [this] {
        (b_is_face_culling_enabled ? glDisable : glEnable)(GL_CULL_FACE);
        b_is_face_culling_enabled = !b_is_face_culling_enabled;
    });
    do_associate_action_to_key(GLFW_KEY_Z, false, [this] {
        glPolygonMode(GL_FRONT_AND_BACK, b_is_wireframe_enabled ? GL_FILL : GL_LINE);
        b_is_wireframe_enabled = !b_is_wireframe_enabled;
    });
}

void EventHandler::do_associate_action_to_key(int key, bool repeatable, const KeyAction& action) {
    key_actions.emplace(key, action);
    if(repeatable) { repeatable_keys.emplace(key, false); }
}

void EventHandler::do_dissociate_key_action(int key) {
    key_actions.erase(key);
    repeatable_keys.erase(key);
}

void EventHandler::do_poll_and_handle_events() {
    glfwPollEvents();

    float temp_time = glfwGetTime();
    delta = temp_time - time;
    time = temp_time;

    /* Repeatable Keys */
    for(const auto& [key, is_active] : repeatable_keys) {
        if(is_active) { key_actions[key](); }
    }
}

void EventHandler::do_set_active_camera(Camera* camera) {
    if(camera == nullptr) {
        /* Remove callbacks if there is no active camera */
        if(active_camera != nullptr) {
            do_dissociate_key_action(GLFW_KEY_W);
            do_dissociate_key_action(GLFW_KEY_A);
            do_dissociate_key_action(GLFW_KEY_S);
            do_dissociate_key_action(GLFW_KEY_D);
            do_dissociate_key_action(GLFW_KEY_SPACE);
            do_dissociate_key_action(GLFW_KEY_C);
        }
    } else {
        /* Set up camera movement callbacks */
        if(active_camera == nullptr) {
            do_associate_action_to_key(GLFW_KEY_W, true, [this] {
                active_camera->move_around(MovementDirection::FORWARD);
            });
            do_associate_action_to_key(GLFW_KEY_A, true, [this] {
                active_camera->move_around(MovementDirection::LEFT);
            });
            do_associate_action_to_key(GLFW_KEY_S, true, [this] {
                active_camera->move_around(MovementDirection::BACKWARD);
            });
            do_associate_action_to_key(GLFW_KEY_D, true, [this] {
                active_camera->move_around(MovementDirection::RIGHT);
            });
            do_associate_action_to_key(GLFW_KEY_SPACE, true, [this] {
                active_camera->move_around(MovementDirection::UPWARD);
            });
            do_associate_action_to_key(GLFW_KEY_C, true, [this] {
                active_camera->move_around(MovementDirection::DOWNWARD);
            });
        }
    }

    active_camera = camera;
}

void EventHandler::do_handle_window_size_event(int width, int height) {
    Window::update_size(width, height);

    if(active_camera != nullptr) { active_camera->update_projection_matrix(); }
}

void EventHandler::do_handle_framebuffer_size_event(int width, int height) {
    glViewport(0, 0, width, height);
}

void EventHandler::do_handle_key_press_event(int key) {
    std::unordered_map<int, bool>::iterator repeatable_key_iterator = repeatable_keys.find(key);

    if(repeatable_key_iterator != repeatable_keys.end()) {
        repeatable_key_iterator->second = true;
    } else {
        std::unordered_map<int, KeyAction>::iterator action_iterator = key_actions.find(key);
        if(action_iterator != key_actions.end()) { action_iterator->second(); }
    }
}

void EventHandler::do_handle_key_release_event(int key) {
    std::unordered_map<int, bool>::iterator repeatable_key_iterator = repeatable_keys.find(key);
    if(repeatable_key_iterator != repeatable_keys.end()) { repeatable_key_iterator->second = false; }
}

void EventHandler::do_handle_cursor_position_event(int position_x, int position_y) {
    if(active_camera != nullptr && !b_is_cursor_visible) {
        active_camera->look_around(position_y - mouse_position.y, position_x - mouse_position.x);
    }

    mouse_position.x = position_x;
    mouse_position.y = position_y;
}

void EventHandler::do_handle_mouse_button_event(int button, int action) {
    if(action == GLFW_PRESS) {
        if(button == GLFW_MOUSE_BUTTON_LEFT) {
            if(left_click_func != nullptr) {
                left_click_func();
            }
        }
    }
}
