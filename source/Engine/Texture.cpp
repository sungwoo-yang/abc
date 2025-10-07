/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author TODO Your Name
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Texture.hpp"

#include "CS200/IRenderer2D.hpp"
#include "CS200/Image.hpp"
#include "Engine.hpp"
#include "OpenGL/GL.hpp"

namespace CS230
{
    Texture::Texture(const std::filesystem::path& file_name)
    {
        CS200::Image image(file_name, true);
        size = image.GetSize();
        textureHandle = OpenGL::CreateTextureFromImage(image);
    }

    Texture::Texture(OpenGL::TextureHandle given_texture, Math::ivec2 the_size) : textureHandle(given_texture), size(the_size) {}

    Texture::~Texture()
    {
        if (textureHandle != 0)
        {
            OpenGL::GL::DeleteTextures(1, &textureHandle);
        }
    }

    Texture::Texture(Texture&& temporary) noexcept : textureHandle(temporary.textureHandle), size(temporary.size)
    {
        temporary.textureHandle = 0;
        temporary.size = { 0, 0 };
    }

    Texture& Texture::operator=(Texture&& temporary) noexcept
    {
        std::swap(textureHandle, temporary.textureHandle);
        std::swap(size, temporary.size);
        return *this;
    }

    void Texture::Draw(const Math::TransformationMatrix& display_matrix, unsigned int color)
    {
        Engine::GetRenderer2D().DrawQuad(display_matrix, textureHandle, { 0, 0 }, { 1, 1 }, color);
    }

    void Texture::Draw(const Math::TransformationMatrix& display_matrix, Math::ivec2 texel_position, Math::ivec2 frame_size, unsigned int color)
    {
        Math::vec2 uv_min = { static_cast<double>(texel_position.x) / size.x, static_cast<double>(texel_position.y) / size.y };
        Math::vec2 uv_max = { static_cast<double>(texel_position.x + frame_size.x) / size.x, static_cast<double>(texel_position.y + frame_size.y) / size.y };
        Engine::GetRenderer2D().DrawQuad(display_matrix, textureHandle, uv_min, uv_max, color);
    }

    Math::ivec2 Texture::GetSize() const
    {
        return size;
    }
}


