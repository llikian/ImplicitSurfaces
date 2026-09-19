# Global Project Configuration
cmake_minimum_required(VERSION 3.26)
project(ModGeom)

# Find Packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# Sources, Includes & Libraries
set(LIBRARY_SOURCES
    lib/glad/src/glad.c

    lib/imgui/imgui.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/backends/imgui_impl_glfw.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
    lib/imgui/misc/cpp/imgui_stdlib.cpp

    lib/stb/stb_image.cpp

    lib/tinygltf/tiny_gltf.cc
)

set(SOURCES
    src/AABB.cpp
    src/Blob.cpp
    src/Surface.cpp

    # Assets Module
    src/assets/Camera.cpp
    src/assets/Shader.cpp

    # Engine Module
    src/engine/callbacks.cpp
    src/engine/EventHandler.cpp
    src/engine/Window.cpp

    # Maths Module
    src/maths/functions.cpp
    src/maths/geometry.cpp
    src/maths/mat3.cpp
    src/maths/mat4.cpp
    src/maths/transforms.cpp
    src/maths/trigonometry.cpp

    # Mesh Module
    src/mesh/Attribute.cpp
    src/mesh/Mesh.cpp
    src/mesh/primitives.cpp

    # Utility Module
    include/utility/ansi.hpp
    include/utility/HeapArray.hpp
    src/utility/gl_enums.cpp
    src/utility/LifetimeLogger.cpp
    src/utility/Random.cpp

    # Libraries
    ${LIBRARY_SOURCES}
)

set(INCLUDES
    include

    # Libraries
    lib/glad/include
    lib/imgui
    lib/imgui/backends
    lib/imgui/misc/cpp
    lib/stb
    lib/tinygltf
)

set(LIBRARIES
    glfw
    dl
    pthread
    X11
    Xxf86vm
    Xrandr
    Xi
)

# Compiler Options
set(CMAKE_CXX_STANDARD 23)

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

set_source_files_properties(${LIBRARY_SOURCES} PROPERTIES COMPILE_FLAGS "-w") # Don't Show Warnings for Library Sources

# CMake Options
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Output compile_commands.json

# Set Directory for Binary Files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# Executable
add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES} src/applications/Application.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARIES})

