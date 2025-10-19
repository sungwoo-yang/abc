/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "TextureManager.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "Engine.hpp"
#include "Logger.hpp"
#include "OpenGL/Framebuffer.hpp"
#include "OpenGL/GL.hpp"
#include "Texture.hpp"
#include <algorithm>

std::shared_ptr<CS230::Texture> CS230::TextureManager::Load(const std::filesystem::path& file_name)
{
    auto it = textures.find(file_name);
    if (it != textures.end())
    {
        return it->second;
    }

    Engine::GetLogger().LogVerbose("Loading new texture: " + file_name.string());
    std::shared_ptr<Texture> new_texture(new Texture(file_name));

    textures[file_name] = new_texture;
    return new_texture;
}

void CS230::TextureManager::Unload()
{
    textures.clear();
    Engine::GetLogger().LogEvent("TextureManager: All cached textures unloaded.");
}

namespace
{
    struct RenderToTextureState
    {
        OpenGL::FramebufferWithColor Target{};
        Math::ivec2                  Size{};
        std::array<GLfloat, 4>       ClearColor{};
        std::array<GLint, 4>         Viewport{};
    };

    RenderToTextureState g_CurrentRenderState;
}

void CS230::TextureManager::StartRenderTextureMode(int width, int height)
{
    auto& renderer_2d = Engine::GetRenderer2D();

    renderer_2d.EndScene();

    g_CurrentRenderState.Size   = { width, height };
    g_CurrentRenderState.Target = OpenGL::CreateFramebufferWithColor(g_CurrentRenderState.Size);

    GL::GetFloatv(GL_COLOR_CLEAR_VALUE, g_CurrentRenderState.ClearColor.data());
    GL::GetIntegerv(GL_VIEWPORT, g_CurrentRenderState.Viewport.data());

    GL::BindFramebuffer(GL_FRAMEBUFFER, g_CurrentRenderState.Target.Framebuffer);
    GL::Viewport(0, 0, width, height);
    GL::ClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    GL::Clear(GL_COLOR_BUFFER_BIT);

    const auto ndc_matrix = Math::ScaleMatrix({ 1.0, -1.0 }) * CS200::build_ndc_matrix(g_CurrentRenderState.Size);
    renderer_2d.BeginScene(ndc_matrix);
}

std::shared_ptr<CS230::Texture> CS230::TextureManager::EndRenderTextureMode()
{
    auto& renderer_2d = Engine::GetRenderer2D();

    renderer_2d.EndScene();

    GL::BindFramebuffer(GL_FRAMEBUFFER, 0);

    GL::Viewport(g_CurrentRenderState.Viewport[0], g_CurrentRenderState.Viewport[1], g_CurrentRenderState.Viewport[2], g_CurrentRenderState.Viewport[3]);
    GL::ClearColor(g_CurrentRenderState.ClearColor[0], g_CurrentRenderState.ClearColor[1], g_CurrentRenderState.ClearColor[2], g_CurrentRenderState.ClearColor[3]);

    OpenGL::TextureHandle     texture_handle = g_CurrentRenderState.Target.ColorAttachment;
    OpenGL::FramebufferHandle fbo_handle     = g_CurrentRenderState.Target.Framebuffer;

    GL::DeleteFramebuffers(1, &fbo_handle);

    std::shared_ptr<Texture> texture(new Texture(texture_handle, g_CurrentRenderState.Size));

    const auto& env         = Engine::GetWindowEnvironment();
    Math::ivec2 window_size = { static_cast<int>(env.DisplaySize.x), static_cast<int>(env.DisplaySize.y) };
    renderer_2d.BeginScene(CS200::build_ndc_matrix(window_size));

    return texture;
}
