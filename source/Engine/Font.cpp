/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Font.hpp"

#include "CS200/Image.hpp"
#include "Engine.hpp"
#include "Error.hpp"
#include "Matrix.hpp"
#include "TextureManager.hpp"
#include <algorithm>

namespace CS230
{
    namespace
    {
        struct CachedText
        {
            std::shared_ptr<Texture> texture;
            uint64_t                 last_frame_used = 0;
        };

        static std::unordered_map<std::string, CachedText> text_cache;
        static uint64_t                                    last_cleanup_frame = 0;
    }

    Font::Font(const std::filesystem::path& file_name)
    {
        CS200::Image image(file_name, false);
        if (image.GetSize().x == 0 || image.GetSize().y == 0 || image.data()[0] != 0xFFFFFFFF)
        {
            throw_error_message("Invalid font file format: " + file_name.string());
        }

        fontTexture = Engine::GetTextureManager().Load(file_name);

        int         last_x_pos   = 1;
        CS200::RGBA last_color   = image.data()[0];
        char        current_char = ' ';

        for (int x = 1; x < image.GetSize().x; ++x)
        {
            if (image.data()[x] != last_color)
            {
                characterRects[current_char] = Math::irect{
                    { last_x_pos,                 0 },
                    {          x, image.GetSize().y }
                };
                last_x_pos = x;
                last_color = image.data()[x];
                current_char++;
            }
        }
        characterRects[current_char] = Math::irect{
            {        last_x_pos,                 0 },
            { image.GetSize().x, image.GetSize().y }
        };
    }

    Math::ivec2 Font::MeasureText(const std::string& text) const
    {
        int total_width = 0;
        int max_height  = 0;
        for (const char c : text)
        {
            if (characterRects.count(c))
            {
                const auto& rect = characterRects.at(c);
                total_width += static_cast<int>(rect.Size().x);
                if (static_cast<int>(rect.Size().y) > max_height)
                {
                    max_height = static_cast<int>(rect.Size().y);
                }
            }
        }
        return { total_width, max_height };
    }

    std::shared_ptr<Texture> Font::PrintToTexture(const std::string& text, CS200::RGBA color)
    {
        const uint64_t current_frame = Engine::GetWindowEnvironment().FrameCount;
        if (current_frame > last_cleanup_frame)
        {
            last_cleanup_frame = current_frame;
            for (auto it = text_cache.begin(); it != text_cache.end();)
            {
                if (it->second.texture.use_count() == 1 && (current_frame - it->second.last_frame_used) > 60)
                {
                    it = text_cache.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        const std::string cache_key = text + std::to_string(color);
        if (text_cache.count(cache_key))
        {
            text_cache[cache_key].last_frame_used = current_frame;
            return text_cache[cache_key].texture;
        }

        const Math::ivec2 text_size = MeasureText(text);
        if (text_size.x == 0 || text_size.y == 0)
        {
            return nullptr;
        }

        TextureManager::StartRenderTextureMode(text_size.x, text_size.y);

        int current_x = 0;
        for (const char c : text)
        {
            if (characterRects.count(c))
            {
                const Math::irect& char_rect      = characterRects.at(c);
                const int          char_width     = static_cast<int>(char_rect.Size().x);
                const int          char_height    = static_cast<int>(char_rect.Size().y);
                const Math::ivec2  texel_position = { static_cast<int>(char_rect.Left()), static_cast<int>(char_rect.Top()) };

                Math::TransformationMatrix transform = Math::TranslationMatrix(Math::vec2{ static_cast<double>(current_x), 0.0 });
                fontTexture->Draw(transform, texel_position, { char_width, char_height });

                current_x += char_width;
            }
        }

        auto new_texture = TextureManager::EndRenderTextureMode();
        if (new_texture)
        {
            text_cache[cache_key] = { new_texture, current_frame };
        }
        return new_texture;
    }
}
