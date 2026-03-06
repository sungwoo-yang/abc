/*
 * Rudy Castan
 * Graphics Library
 * CS250
 * Spring 2026
 */
#pragma once

namespace CS250
{
    struct [[nodiscard]] Color
    {
    public:
        using ColorChannel = unsigned char;
        using RGBA32       = unsigned int;

        ColorChannel Red   = 0;
        ColorChannel Green = 0;
        ColorChannel Blue  = 0;
        ColorChannel Alpha = 255;

    public:
        Color() = default;
        constexpr Color(ColorChannel red, ColorChannel green, ColorChannel blue, ColorChannel alpha = 255) noexcept;
        constexpr explicit Color(RGBA32 color) noexcept;
    };

    static_assert(sizeof(Color) == sizeof(Color::RGBA32),
                  "Color should be the same as 4 bytes, so we can easily treat it as an RGBA int.");
}


namespace CS250
{
    constexpr Color::Color(ColorChannel red, ColorChannel green, ColorChannel blue, ColorChannel alpha) noexcept
        : Red(red), Green(green), Blue(blue), Alpha(alpha)
    {
    }

    constexpr Color::Color(RGBA32 color) noexcept
        : Red(static_cast<ColorChannel>((color & 0xff000000) >> 24)),
          Green(static_cast<ColorChannel>((color & 0x00ff0000) >> 16)),
          Blue(static_cast<ColorChannel>((color & 0x0000ff00) >> 8)),
          Alpha(static_cast<ColorChannel>((color & 0x000000ff) >> 0))
    {
    }
}
