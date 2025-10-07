/**
 * \file
 * \author Rudy Castan
 * \author Sungwoo Yang
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
        const auto full_path = assets::locate_asset(image_path);
        if (full_path.empty())
        {
            throw_error_message("Failed to find image file: ", image_path.string());
        }

        stbi_set_flip_vertically_on_load(flip_vertical);

        int            width, height, channels;
        unsigned char* loaded_pixels = stbi_load(full_path.string().c_str(), &width, &height, &channels, 4);

        if (loaded_pixels == nullptr)
        {
            throw_error_message("Failed to load image: ", full_path.string());
        }

        this->pixels = reinterpret_cast<RGBA*>(loaded_pixels);
        this->size   = { width, height };
    }

    Image::~Image()
    {
        if (pixels != nullptr)
        {
            stbi_image_free(pixels);
            pixels = nullptr;
        }
    }

    Image::Image(Image&& temporary) noexcept
    {
        pixels = temporary.pixels;
        size   = temporary.size;

        temporary.pixels = nullptr;
        temporary.size   = { 0, 0 };
    }

    Image& Image::operator=(Image&& temporary) noexcept {
        if (this != &temporary) {
            if (pixels != nullptr) {
                stbi_image_free(pixels);
            }
            
            pixels = temporary.pixels;
            size = temporary.size;

            temporary.pixels = nullptr;
            temporary.size = {0, 0};
        }

        return *this;
    }

    const RGBA* Image::data() const noexcept
    {
        return pixels;
    }

    RGBA* Image::data() noexcept {
        return pixels;
    }

    Math::ivec2 Image::GetSize() const noexcept
    {
        return size;
    }
}
