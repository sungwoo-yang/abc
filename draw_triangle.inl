// Name       : Sungwoo Yang
// Assignment : Triangle Rasterization
// Course     : CS250 Computer Graphics II
// Term & Year: 2026 Spring
#pragma once

#include "vec2.h"

namespace CS250
{
    namespace helper
    {
        constexpr float min3(float a, float b, float c) noexcept
        {
            float m = (a < b) ? a : b;
            return (m < c) ? m : c;
        }

        constexpr float max3(float a, float b, float c) noexcept
        {
            float m = (a > b) ? a : b;
            return (m > c) ? m : c;
        }

        struct Edge
        {
            float a, b, c;
            bool tl;

            constexpr Edge(const vec2 &p0, const vec2 &p1) noexcept
                : a(p0.y - p1.y),
                  b(p1.x - p0.x),
                  c((p1.y - p0.y) * p0.x - (p1.x - p0.x) * p0.y),
                  tl((CS250::is_zero(a) && b < 0.0f) || (!CS250::is_zero(a) && a > 0.0f))
            {
            }

            constexpr float evaluate(float x, float y) const noexcept
            {
                return a * x + b * y + c;
            }

            constexpr bool isInside(float eval) const noexcept
            {
                if (CS250::is_zero(eval))
                {
                    return tl;
                }

                return eval > 0.0f;
            }
        };

        constexpr int fast_floor(float v) noexcept
        {
            int i = static_cast<int>(v);
            return (v < static_cast<float>(i)) ? i - 1 : i;
        }

        constexpr int fast_ceil(float v) noexcept
        {
            int i = static_cast<int>(v);
            return (v > static_cast<float>(i)) ? i + 1 : i;
        }
    }
    template <typename COLOR, typename SETPIXEL>
    constexpr void draw_triangle(vec2 pos0, vec2 pos1, vec2 pos2, COLOR fill_value, SETPIXEL setpixel) noexcept
    {
        const float signed_area = (pos1.x - pos0.x) * (pos2.y - pos0.y) - (pos2.x - pos0.x) * (pos1.y - pos0.y);
        if (signed_area < 0.0f)
        {
            return;
        }

        const helper::Edge e0(pos1, pos2);
        const helper::Edge e1(pos2, pos0);
        const helper::Edge e2(pos0, pos1);

        const int minX = helper::fast_floor(helper::min3(pos0.x, pos1.x, pos2.x));
        const int maxX = helper::fast_ceil(helper::max3(pos0.x, pos1.x, pos2.x));
        const int minY = helper::fast_floor(helper::min3(pos0.y, pos1.y, pos2.y));
        const int maxY = helper::fast_ceil(helper::max3(pos0.y, pos1.y, pos2.y));

        float start_x = static_cast<float>(minX) + 0.5f;
        float start_y = static_cast<float>(minY) + 0.5f;

        float eval0 = e0.evaluate(start_x, start_y);
        float eval1 = e1.evaluate(start_x, start_y);
        float eval2 = e2.evaluate(start_x, start_y);

        for (int y = minY; y < maxY; ++y)
        {
            float hEval0 = eval0;
            float hEval1 = eval1;
            float hEval2 = eval2;

            for (int x = minX; x < maxX; ++x)
            {
                if (e0.isInside(hEval0) && e1.isInside(hEval1) && e2.isInside(hEval2))
                {
                    setpixel(x, y, fill_value);
                }

                hEval0 += e0.a;
                hEval1 += e1.a;
                hEval2 += e2.a;
            }

            eval0 += e0.b;
            eval1 += e1.b;
            eval2 += e2.b;
        }
    }
}
