/**
 * \file
 * \author Rudy Castan
 * \author TODO Your Name
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Image.hpp"

#include "Engine/Error.hpp"
#include "Engine/Path.hpp"

#include <stb_image.h>

namespace CS200
{
    Image::Image(const std::filesystem::path& image_path, bool flip_vertical)
    {
        stbi_set_flip_vertically_on_load(flip_vertical);
        auto path_string = assets::locate_asset(image_path).string();
        int width, height, channels;
        pixel_data = reinterpret_cast<RGBA*>(stbi_load(path_string.c_str(), &width, &height, &channels, 4));
        if (pixel_data == nullptr)
        {
            throw_error_message("Failed to load image: " + path_string);
        }
        size = { width, height };
    }

    Image::Image(Image&& temporary) noexcept : pixel_data(temporary.pixel_data), size(temporary.size)
    {
        temporary.pixel_data = nullptr;
        temporary.size = { 0, 0 };
    }

    Image& Image::operator=(Image&& temporary) noexcept
    {
        std::swap(pixel_data, temporary.pixel_data);
        std::swap(size, temporary.size);
        return *this;
    }

    Image::~Image()
    {
        if (pixel_data)
        {
            stbi_image_free(pixel_data);
        }
    }

    const RGBA* Image::data() const noexcept
    {
        return pixel_data;
    }

    RGBA* Image::data() noexcept
    {
        return pixel_data;
    }

    Math::ivec2 Image::GetSize() const noexcept
    {
        return size;
    }
}
