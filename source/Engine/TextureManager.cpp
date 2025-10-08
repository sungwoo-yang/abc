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
#include "OpenGL/GL.hpp"
#include "Texture.hpp"

namespace CS230
{
    Texture* TextureManager::Load(const std::filesystem::path& file_name)
    {
        auto it = textures.find(file_name);
        if (it != textures.end())
        {
            return it->second;
        }

        try
        {
            Texture* texture = new Texture(file_name);
            textures.emplace(file_name, texture);
            Engine::GetLogger().LogEvent("Loaded texture: " + file_name.string());
            return texture;
        }
        catch (const std::exception& e)
        {
            Engine::GetLogger().LogError("Failed to load texture: " + file_name.string());
            return nullptr;
        }
    }

    void TextureManager::Unload()
    {
        for (auto const& [path, texture] : textures)
        {
            delete texture;
        }
        textures.clear();
        Engine::GetLogger().LogEvent("Unloaded all textures.");
    }
}
