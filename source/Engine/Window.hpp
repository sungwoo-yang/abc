/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author TODO Your Name
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#pragma once
#include "CS200/RGBA.hpp"
#include "Vec2.hpp"
#include <functional>
#include <gsl/gsl>
#include <string_view>

struct SDL_Window;
typedef void*           SDL_GLContext;
typedef union SDL_Event SDL_Event;

namespace CS230
{
    class Window
    {
    public:
        void          Start(std::string_view title);
        void          Update();
        bool          IsClosed() const;
        Math::ivec2   GetSize() const;
        void          Clear(CS200::RGBA color);
        void          ForceResize(int desired_width, int desired_height);
        SDL_Window*   GetSDLWindow() const;
        SDL_GLContext GetGLContext() const;

        using WindowEventCallback = std::function<void(const SDL_Event&)>;
        void SetEventCallback(WindowEventCallback callback);

        Window() = default;
        ~Window();

        Window(const Window&)                = delete;
        Window& operator=(const Window&)     = delete;
        Window(Window&&) noexcept            = delete;
        Window& operator=(Window&&) noexcept = delete;

    private:
        // TODO private data and helper member functions here
        // TODO default size should be 800x600
    };
}
