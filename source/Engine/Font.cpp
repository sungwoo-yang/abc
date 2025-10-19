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
    Font::Font(const std::filesystem::path& file_name)
    {
        fontTexture = Engine::GetTextureManager().Load(file_name);
        CS200::Image image(file_name, false);
        if (image.GetSize().x == 0 || image.GetSize().y == 0)
            throw_error_message("Failed to load font image: " + file_name.string());

        const int width      = image.GetSize().x;
        const int height     = image.GetSize().y;
        const int char_width = width / num_chars;

        for (int i = 0; i < num_chars; ++i)
        {
            char_rects[i].point_1 = { i * char_width, height };
            char_rects[i].point_2 = { (i + 1) * char_width, 0 };
        }
    }

    std::shared_ptr<Texture> Font::PrintToTexture(const std::string& text, CS200::RGBA color)
    {
        if (text.empty())
            return nullptr;

        CleanCache();

        std::stringstream ss;
        ss << text << "_" << std::hex << color;
        const std::string key = ss.str();

        if (auto it = textureCache.find(key); it != textureCache.end())
        {
            it->second.last_used_frame = Engine::GetWindowEnvironment().FrameCount;
            return it->second.texture;
        }

        const Math::ivec2 size = MeasureText(text);
        if (size.x <= 0 || size.y <= 0)
            return nullptr;

        TextureManager::StartRenderTextureMode(size.x, size.y);

        float cursor_x = 0.0f;
        for (char c : text)
        {
            if (c < first_char || c > last_char)
                continue;

            const Math::irect& rect      = char_rects[c - first_char];
            const Math::ivec2  texel_pos = { rect.point_1.x, rect.point_2.y };
            const Math::ivec2  char_size = { static_cast<int>(rect.Size().x), static_cast<int>(rect.Size().y) };

            const float half_w = static_cast<float>(char_size.x) * 0.5f;
            const float half_h = static_cast<float>(char_size.y) * 0.5f;

            const auto to_center = Math::TranslationMatrix(Math::vec2(-half_w, -half_h));
            const auto y_flip    = Math::ScaleMatrix(Math::vec2{ 1.0f, -1.0f });
            const auto to_bottom = Math::TranslationMatrix(Math::vec2(half_w, half_h));
            const auto flip_quad = to_bottom * y_flip * to_center;

            const auto place = Math::TranslationMatrix(Math::vec2(cursor_x + half_w, half_h));

            fontTexture->Draw(place * flip_quad, texel_pos, char_size, color);
            cursor_x += static_cast<float>(char_size.x);
        }

        auto new_tex = TextureManager::EndRenderTextureMode();
        if (new_tex)
        {
            textureCache[key] = { new_tex, Engine::GetWindowEnvironment().FrameCount };
        }

        return new_tex;
    }

    Math::ivec2 Font::MeasureText(const std::string& text)
    {
        if (text.empty())
            return { 0, 0 };

        int total_w = 0;
        int max_h   = 0;

        for (char c : text)
        {
            if (c < first_char || c > last_char)
                continue;

            const Math::irect& rect = char_rects[c - first_char];
            total_w += static_cast<int>(rect.Size().x);
            max_h = std::max(max_h, static_cast<int>(rect.Size().y));
        }

        return { total_w, max_h };
    }

    void Font::CleanCache()
    {
        const uint64_t current_frame = Engine::GetWindowEnvironment().FrameCount;
        for (auto it = textureCache.begin(); it != textureCache.end();)
        {
            if (it->second.texture.use_count() <= 1 && current_frame > it->second.last_used_frame + 60)
            {
                it = textureCache.erase(it);
            }
            else
                ++it;
        }
    }
}
