#include "Background.hpp"
#include "Engine/Engine.hpp"
#include "Engine/TextureManager.hpp"

void Background::Add(const std::filesystem::path& texture_path, double speed)
{
    backgrounds.push_back(ParallaxLayer{ Engine::GetTextureManager().Load(texture_path), speed });
}

void Background::Unload()
{
    backgrounds.clear();
}

void Background::Draw(const CS230::Camera& camera)
{
    for (ParallaxLayer& background : backgrounds)
    {
        Math::vec2              inverted_position = -Math::vec2{ camera.GetPosition().x * background.speed, camera.GetPosition().y };
        Math::TranslationMatrix new_matrix        = Math::TranslationMatrix(inverted_position);
        background.texture->Draw(new_matrix);
    }
}

Math::ivec2 Background::GetSize()
{
    if (!backgrounds.empty())
    {
        return backgrounds.back().texture->GetSize();
    }
    return { 0, 0 };
}