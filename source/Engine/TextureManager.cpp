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
        auto it = textureCache.find(file_name);
        if (it != textureCache.end())
        {
            Engine::GetLogger().LogDebug("Texture '" + file_name.string() + "' loaded from cache.");
            return it->second.get();
        }

        try
        {
            Engine::GetLogger().LogEvent("Loading texture '" + file_name.string() + "' from file.");
            std::unique_ptr<Texture> newTexture(new Texture(file_name));
            Texture*                 texturePtr = newTexture.get();
            textureCache[file_name]             = std::move(newTexture);
            return texturePtr;
        }
        catch (const std::exception& e)
        {
            Engine::GetLogger().LogError("Failed to load texture '" + file_name.string() + "': " + e.what());
            return nullptr;
        }
    }

    void TextureManager::Unload()
    {
        textureCache.clear();
        Engine::GetLogger().LogEvent("All textures unloaded and cache cleared.");
    }
}
