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
        if (textures.find(file_name) == textures.end())
        {
            textures[file_name] = std::make_unique<Texture>(file_name);
            Engine::GetLogger().LogEvent("Loaded texture: " + file_name.string());
        }
        return textures[file_name].get();
    }

    void TextureManager::Unload()
    {
        textures.clear();
        Engine::GetLogger().LogEvent("All textures unloaded.");
    }

    // Define texture_map member variable
    std::map<std::filesystem::path, std::unique_ptr<Texture>> TextureManager::textures;
}
