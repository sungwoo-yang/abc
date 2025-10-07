/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \author Sungwoo Yang
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

        textureHandle = OpenGL::CreateTextureFromImage(image);
        size          = image.GetSize();
    }

    Texture::Texture(OpenGL::TextureHandle given_texture, Math::ivec2 the_size) : textureHandle(given_texture), size(the_size)
    {
    }

    Texture::~Texture()
    {
        if (textureHandle != 0)
        {
            GL::DeleteTextures(1, &textureHandle);
            textureHandle = 0;
        }
    }

    void Texture::Draw(const Math::TransformationMatrix& display_matrix, unsigned int color)
    {
        Math::TransformationMatrix transform = display_matrix * Math::ScaleMatrix(static_cast<Math::vec2>(size));
        Engine::GetRenderer2D().DrawQuad(transform, textureHandle, { 0.0f, 0.0f }, { 1.0f, 1.0f }, color);
    }

    void Texture::Draw(const Math::TransformationMatrix& display_matrix, Math::ivec2 texel_position, Math::ivec2 frame_size, unsigned int color)
    {
        Math::TransformationMatrix transform = display_matrix * Math::ScaleMatrix(static_cast<Math::vec2>(frame_size));

        Math::vec2 uv_bl, uv_tr;
        if (size.x > 0 && size.y > 0)
        {
            uv_bl.x = static_cast<double>(texel_position.x) / size.x;
            uv_bl.y = static_cast<double>(texel_position.y) / size.y;
            uv_tr.x = static_cast<double>(texel_position.x + frame_size.x) / size.x;
            uv_tr.y = static_cast<double>(texel_position.y + frame_size.y) / size.y;
        }

        Engine::GetRenderer2D().DrawQuad(transform, textureHandle, uv_bl, uv_tr, color);
    }

    Math::ivec2 Texture::GetSize() const
    {
        return size;
    }

    Texture::Texture(Texture&& temporary) noexcept : textureHandle(temporary.textureHandle), size(temporary.size)
    {
        temporary.textureHandle = 0;
        temporary.size          = { 0, 0 };
    }

    Texture& Texture::operator=(Texture&& temporary) noexcept
    {
        if (this != &temporary)
        {
            if (textureHandle != 0)
            {
                GL::DeleteTextures(1, &textureHandle);
            }
            textureHandle           = temporary.textureHandle;
            size                    = temporary.size;
            temporary.textureHandle = 0;
            temporary.size          = { 0, 0 };
        }
        return *this;
    }
}
