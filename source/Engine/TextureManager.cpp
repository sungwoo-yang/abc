/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author TODO Your Name
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "TextureManager.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "Engine.hpp"
#include "Logger.hpp"
#include "OpenGL/GL.hpp"
#include "Texture.hpp"

namespace CS230
{
    Texture* TextureManager::Load(const std::filesystem::path& file_name)
    {
        if (textures.find(file_name) != textures.end())
        {
            return textures[file_name].get();
        }

        try
        {
            auto texture = std::make_unique<Texture>(file_name);
            textures[file_name] = std::move(texture);
            Engine::GetLogger().LogEvent("Loaded texture: " + file_name.string());
            return textures[file_name].get();
        }
        catch (const std::exception& e)
        {
            Engine::GetLogger().LogError("Failed to load texture: " + file_name.string() + " - " + e.what());
            return nullptr;
        }
    }

    void TextureManager::Unload()
    {
        textures.clear();
        Engine::GetLogger().LogEvent("Unloaded all textures.");
    }
}
