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
        auto it = textures.find(file_name);
        if (it != textures.end())
        {
            return it->second.get();
        }

        try
        {
            auto     texture = std::unique_ptr<Texture>(new Texture(file_name));
            Texture* ptr     = texture.get();
            textures.emplace(file_name, std::move(texture));
            Engine::GetLogger().LogEvent("Loaded texture: " + file_name.string());
            return ptr;
        }
        catch (const std::exception& e)
        {
            Engine::GetLogger().LogError("Failed to load texture: " + file_name.string());
            return nullptr;
        }
    }

    void TextureManager::Unload()
    {
        textures.clear();
        Engine::GetLogger().LogEvent("Unloaded all textures.");
    }
}
