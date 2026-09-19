# OpenGL Engine
![Screenshot from 2025-01-19 14-47-40.png](screenshots/Screenshot%20from%202025-01-19%2014-47-40.png)

## Project Description
This project is an implementation of a graphics engine using OpenGL with the GLAD implementation.
It uses GLFW for window and event handling and stb_image for loading textures and other images.

This project is for learning purposes. For OpenGL I am using [Joey de Vries's tutorials](https://learnopengl.com/)
as a reference and for more general graphics programming concepts I am also using the [Real-Time Rendering 4th Edition](https://www.realtimerendering.com/)
book.

### Structure
```
./
├─ bin/         # Binary Files
├─ data/        # Assets (textures, etc...)
├─ include/     # Header files (.hpp and .tpp files)
├─ lib/         # Source code of libraries
├─ screenshots/ # Screenshots of the application
├─ shaders/     # Shader code files (.vert, .frag, etc...)
└─ src/         # Source code files (.cpp files)
   ├─ applications/ # Code for the different applications
   ├─ engine/       # Code concerning the engine itself
   ├─ maths/        # Code regarding maths operations and structures (vectors, matrices, etc...)
   └─ mesh/         # Code regarding the definition and creation of meshes
```

### Functionalities
The engine currently supports a few functionalities, separated in multiple classes. First and most
important, ApplicationBase is a virtual class that is meant to provide the skeleton for any application
using this engine. It initializes the window using the Window class which I'll ellaborate on further,
defines virtual static methods used for handling the GLFW callbacks (which are used to handle events such
as mouth movement, window resizing, key presses and such).

Another thing ApplicationBase handles is how key events are handled. The manner in which they are is
different regarding if the key is meant to be repeatable (if the key is held, the associated action
is repeated) or not. The `std::unordered_map<int, bool> repeatableKeys` member of the class contains all the
keys that are repeatable, it is the responsibility of the application implementing the ApplicationBase
class to fill the map. It associates an integer (corresponding to the GLFW code of a specific key) to
a boolean, when the boolean is true, it means the key is currently being held and so the associated
action should be called. For non repeatable keys it is way more simple, there is simply a queue of the
keys that were pressed that is handled each frame.

So in short, when there's a key callback, we first check if the pressed key is repeatable, if it is,
then we set the associated boolean in the `repeatableKeys` map to true if it was a key press, and to
false if it was a key release (conveniently GLFW_PRESS is equal to 1 and GLFW_RELEASE is equal to 0).
If the pressed key wasn't repeatable we simply add its key code to the queue.

The Window class initializes the libraries (OpenGL, GLFW, GLAD & stb) and the GLFW window. It also
provides functionality for getting the width and height of the window as well as its size ratio.

Another very important part of the core engine is the Shader class, its job is to load, compile and
link shaders into a shader program. It also allows to use said shader program and to modify the values
of the uniforms in it. Once the shader program is created we use OpenGL's `glGetActiveUniform` in order
to fill an unordered map which associates the name of each uniform in the program to its location.
This way we only make one call to `glGetUniformLocation` for each uniform and when the `setUniform`
method of Shader is called, it looks up its location in the map which is usually just a O(1) operation.

Movement and looking around is handled by the Camera class, it's a very basic implementation of a
third-person camera using two angles: yaw and pitch in order to represent the direction its looking
in. The yaw angle is the "left-right" angle, it represents a rotation around the y axis and the pitch
angle is the "up-down" angle and represents a rotation around the x axis.

The camera class also defines and updates its associated view matrix. It is defined by the following
formula:
```math
View = \begin{pmatrix}
    \vec{right}.x & \vec{up}.x & -\vec{front}.x & 0 \\
    \vec{right}.y & \vec{up}.y & -\vec{front}.y & 0 \\
    \vec{right}.z & \vec{up}.z & -\vec{front}.z & 0 \\
    -\vec{right} \cdot \vec{position} & -\vec{up} \cdot \vec{position} & \vec{front} \cdot \vec{position} & 1
\end{pmatrix}
```

\
With `position` being the position of the camera and the front, right and up vectors being defined as
follows:
```math
\vec{front}=\begin{pmatrix} \cos(pitch)\times\cos(yaw) \\ \sin(pitch) \\ \cos(pitch)\times\sin(yaw) \end{pmatrix},\space
\vec{right}=\widehat{\vec{front} \times \vec{worldUp}},\space
\vec{up}=\widehat{\vec{right} \times \vec{front}}
```

\
And just to be clear with notations:
- $`\vec{u}`$ $`\cdot`$ $`\vec{v}`$ is the dot product of $`\vec{u}`$ and $`\vec{v}`$
- $`\vec{u}`$ $`\times`$ $`\vec{v}`$ is the cross product of $`\vec{u}`$ and $`\vec{v}`$
- $`\widehat{v}=\frac{\vec{v}}{\left\| \vec{v} \right\|}`$, the normalization of $`\vec{v}`$
- $`\vec{worldUp}`$ is the direction of "up" in the world. For now it's just a constant going in the direction
of the y-axis: $`\vec{worldUp}=\begin{pmatrix} 0 \\ 1 \\ 0 \end{pmatrix}`$

The Mesh class allows to define and draw meshes by defining their vertex attributes. One mesh is defined
by a primitive (GL_TRIANGLES, GL_LINES, etc...) and by up to 4 of these vertex attributes: position,
normal, color and texture coordinates. A vertex's attributes need to be defined in that particular order
since all of the vertex data is stored in one array. You can also use an EBO by defining the indices
of each triangles (or lines) to avoid repeating data.

Additionally, the Meshes namespace contains the definitions of multiple basic meshes, such as cubes,
spheres, grids or planes.

There are also some other more minor classes, namely Image which loads images, Texture which creates
and allows to bind OpenGL textures and a few structures to hold information for a few different types
of lights: Light, DirectionalLight, PointLight, SpotLight and FlashLight.

Finally there's the maths module of the engine which implements all of the maths structures and functions
needed for 3D rendering, the main ones being vectors (vec2, vec3, vec4) and matrices (mat4). It implements
usual mathematical functions such as normalizing a vector, calculating its length or the dot and cross
products. It also defines all of the basic transformation matrices such as scaling, rotation, translation
and perspective matrices.

### Controls
```
Close Application: Escape
Toggle Wireframe Mesh: W
Toggle Cullface: C
Toggle Cursor (Allows to move the camera): TAB
Toggle Drawing Axes: A
Toggle Drawing Grid: G
Toggle Drawing Ground: H
Toggle Global Lighting: J
Move: Z Q S D
Move Upwards: Space
Move Downwards: Left Shift
```

## Setup
### Dependencies
On a Debian system you can install the needed dependencies using:
```shell
sudo apt install g++ cmake make libglfw3-dev libxxf86vm-dev libxi-dev -y
```

### Clone Repository
```shell
git clone https://github.com/llikian/OpenGL-Engine.git
cd OpenGL-Engine
```

### Build
To build you can simply run the `release.sh` script at the root of the project using:
```shell
bash release.sh
```

You can also manually build it using:
```shell
cmake -B build && \
cmake --build build -j
```

Then you can run it using:
```shell
bin/OpenGL-Engine
```

## Credits
Graphics are handled with [OpenGL](https://www.opengl.org/), using the [GLAD](https://github.com/Dav1dde/glad) implementation.

Window management and event handling are done with [GLFW](https://www.glfw.org/).

Image loading is done with [stb_image](https://github.com/nothings/stb).