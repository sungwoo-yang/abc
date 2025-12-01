# CS230 Web Port Documentation

## 1. Team Information
* **Name:** Sungwoo Yang

## 2. Project Description
This project is a custom 2D Game Engine developed for CS230 (Computer Graphics I). It demonstrates fundamental graphics programming concepts using C++, OpenGL, and SDL2. The engine features a component-based architecture, state management, and an immediate mode renderer capable of handling textured quads, sprite animations, and SDF-based shapes.

The final project includes two distinct game modes demonstrating the engine's capabilities:
* **Mode 1 (Side Scroller):** A platformer featuring a cat character capable of running, jumping, and skidding. It includes gravity, collision detection (AABB), parallax scrolling backgrounds, and interactions with various objects like crates, robots, and portals.
* **Mode 2 (Space Shooter):** An "Asteroids"-style shooter featuring a spaceship with tank controls (thrust, rotate). It includes screen wrapping, particle effects, projectile shooting, and asteroid splitting mechanics.

## 3. Major Technical Challenges
During the porting process to the Web (Emscripten/WebGL 2), the following challenges were encountered:

* **Game Loop Restructuring:** The most significant challenge was converting the traditional infinite `while` loop in C++ to a browser-friendly callback system. The native engine blocks execution, which hangs the browser. This was resolved by refactoring `main.cpp` to use `emscripten_set_main_loop` and separating the frame logic into a standalone `main_loop()` function.
* **File System & Asset Loading:** Native file paths do not exist in the web environment. The build system had to be updated to embed assets into the virtual file system (`--embed-file`). I also had to ensure `std::filesystem` and `stb_image` paths were relative to the virtual root correctly.
* **OpenGL ES Compatibility:** Transitioning from desktop OpenGL to WebGL 2 (OpenGL ES 3.0).
    * Shader versions had to be updated to `#version 300 es` and precision qualifiers (`precision mediump float;`) were added to fragment shaders.
    * Window creation flags in `Window.cpp` needed conditional compilation to request an ES profile context instead of a Core profile.
* **Debug Output:** Standard console logging needed to be routed to the browser's developer console. The build script and CMake configuration were adjusted to enable `ASSERTIONS` and proper error reporting in the web build.

## 4. Modified or Removed Features
To ensure stability and performance on the web platform, the following modifications were made:

* **Infinite Loop Removal:** The blocking `while (!engine.HasGameEnded())` loop in `main.cpp` was removed and replaced with the Emscripten main loop callback mechanism.
* **Window Resizing:** Native window resizing events behave differently on the web. A JavaScript hook (`setWindowSize`) was bound using `EMSCRIPTEN_BINDINGS` to allow the HTML shell to resize the canvas dynamically.
* **OpenGL Context Initialization:** In `Window.cpp`, the context profile mask was changed to `SDL_GL_CONTEXT_PROFILE_ES` when compiling for Emscripten to target WebGL 2.
* **Shader Code:** Shaders (`quad.vert`, `quad.frag`, etc.) were modified to include `#version 300 es` and explicit precision qualifiers required by WebGL SL.

## 5. Build and Run Instructions

### Prerequisites
* CMake (3.21+)
* Emscripten SDK (emsdk) installed and activated.
* Python 3

### Building for Web
1.  Open a terminal in the project root directory.
2.  Run the provided build script:
    ```sh
    python3 scripts/scan_build_project.py --target web --build-type release
    ```
    *Alternatively, using CMake presets directly:*
    ```sh
    cmake --preset web-release
    cmake --build --preset web-release
    ```

### Running Locally
1.  Navigate to the build directory:
    ```sh
    cd build/web-release
    ```
2.  Start a local Python web server:
    ```sh
    python3 -m http.server 8000
    ```
3.  Open a web browser and go to:
    `http://localhost:8000/CS230_fun.html`

## 6. Deployed Web Version
You can play the web version of the project here:
****