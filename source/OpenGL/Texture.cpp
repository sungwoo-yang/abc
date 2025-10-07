/**
 * \file
 * \author Rudy Castan
 * \author Sungwoo Yang
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Texture.hpp"
#include "CS200/Image.hpp"
#include "Environment.hpp"
#include "GL.hpp"

namespace OpenGL
{
    [[nodiscard]] TextureHandle CreateTextureFromMemory(Math::ivec2 size, std::span<const CS200::RGBA> colors, Filtering filtering, Wrapping wrapping) noexcept
    {
        GLuint texture_id = 0;
        GL::GenTextures(1, &texture_id);
        GL::BindTexture(GL_TEXTURE_2D, texture_id);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping));
        GL::TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, colors.data());
        GL::BindTexture(GL_TEXTURE_2D, 0);
        return texture_id;
    }

    [[nodiscard]] TextureHandle CreateTextureFromImage(const CS200::Image& image, Filtering filtering, Wrapping wrapping) noexcept
    {
        const auto                         size = image.GetSize();
        const std::span<const CS200::RGBA> colors(image.data(), size.x * size.y);
        return CreateTextureFromMemory(size, colors, filtering, wrapping);
    }

    [[nodiscard]] TextureHandle CreateRGBATexture(Math::ivec2 size, Filtering filtering, Wrapping wrapping) noexcept
    {
        GLuint texture_id = 0;
        GL::GenTextures(1, &texture_id);
        GL::BindTexture(GL_TEXTURE_2D, texture_id);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping));
        GL::TexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        GL::BindTexture(GL_TEXTURE_2D, 0);
        return texture_id;
    }

    void SetFiltering(TextureHandle texture_handle, Filtering filtering) noexcept
    {
        GL::BindTexture(GL_TEXTURE_2D, texture_handle);
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filtering));
        GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filtering));
        GL::BindTexture(GL_TEXTURE_2D, 0);
    }

    void SetWrapping(TextureHandle texture_handle, Wrapping wrapping, TextureCoordinate coord) noexcept
    {
        GL::BindTexture(GL_TEXTURE_2D, texture_handle);

        if (coord == TextureCoordinate::S || coord == TextureCoordinate::Both)
        {
            GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapping));
        }
        if (coord == TextureCoordinate::T || coord == TextureCoordinate::Both)
        {
            GL::TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapping));
        }

        GL::BindTexture(GL_TEXTURE_2D, 0);
    }

    void DeleteTexture(TextureHandle texture_handle) noexcept
    {
        GL::DeleteTextures(1, &texture_handle);
    }
}
