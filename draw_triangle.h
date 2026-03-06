/*
 * Rudy Castan
 * Graphics Library
 * CS250
 * Spring 2026
 */
#pragma once

#include "vec2.h"

#include <functional>

namespace CS250
{
    // SETPIXEL is treated like a function: void SETPIXEL(int column, int row, COLOR color)
    template <typename COLOR, typename SETPIXEL>
    constexpr void draw_triangle(vec2 P0, vec2 P1, vec2 P2, COLOR fill_value, SETPIXEL setpixel) noexcept;
}

#include "draw_triangle.inl"