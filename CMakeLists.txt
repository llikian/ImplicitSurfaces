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

    lib/imnodes/imnodes.cpp

    lib/stb/stb_image.cpp
)

set(SOURCES
    src/AABB.cpp
    src/Surface.cpp

    # Assets Module
    src/assets/Camera.cpp
    src/assets/Shader.cpp
    src/assets/Texture.cpp

    # Blobs Module
    src/blobs/BezierBlob.cpp
    src/blobs/Blob.cpp
    src/blobs/CapsuleBlob.cpp
    src/blobs/OperationBlob.cpp
    src/blobs/SphereBlob.cpp
    src/blobs/TransformBlob.cpp

    # Engine Module
    src/engine/EventHandler.cpp
    src/engine/Framebuffer.cpp
    src/engine/NEAttribute.cpp
    src/engine/NENode.cpp
    src/engine/NodeEditor.cpp
    src/engine/Window.cpp
    src/engine/callbacks.cpp

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
    include/utility/HeapArray.hpp
    include/utility/ansi.hpp
    src/utility/LifetimeLogger.cpp
    src/utility/Random.cpp
    src/utility/Timer.cpp
    src/utility/gl_enums.cpp

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
    lib/imnodes
    lib/stb
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
add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES} src/Application.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARIES})

