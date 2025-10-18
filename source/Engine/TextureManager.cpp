/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author TODO: Your Name
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
    // TODO implement this function
    return nullptr; // TODO replace this line with the correct return value
}

void CS230::TextureManager::Unload()
{
    // TODO implement this function
}

namespace
{
    // TODO define any necessary internal helper structures or variables here
}

void CS230::TextureManager::StartRenderTextureMode(int width, int height)
{
    // TODO implement this function
}

std::shared_ptr<CS230::Texture> CS230::TextureManager::EndRenderTextureMode()
{
    // TODO implement this function
    return nullptr; // TODO replace this line with the correct return value
}
