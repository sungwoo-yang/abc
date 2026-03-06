/*
 * Rudy Castan
 * Graphics Library
 * CS250
 * Spring 2026
 */
#include "Image.hpp"
#include <algorithm>
#include <cassert>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#ifdef WIN32
#    define STBI_MSC_SECURE_CRT
#endif
#include "stb_image_write.h"

namespace CS250
{
    Image::Image(int pixel_width, int pixel_height, Color color) noexcept
        : width(pixel_width), height(pixel_height), pixels(pixel_width * pixel_height, color)
    {
    }

    void Image::SaveToPNG(const std::string& file_path, bool flip_vertically) const noexcept
    {
        if (pixels.empty())
            return;

        const int stride = 0;
        stbi_flip_vertically_on_write(flip_vertically);
        stbi_write_png(file_path.c_str(), width, height, ChannelsPerColor, &pixels[0], stride);
    }

    void Image::SetPixel(int column, int row, Color color)
    {
        assert(column >= 0 && column < width);
        assert(row >= 0 && row < height);
        pixels[column + row * width] = color;
    }

    void Image::Clear(Color color) { pixels.assign(pixels.size(), color); }
}
