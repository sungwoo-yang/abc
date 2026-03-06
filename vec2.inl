/*
 * Rudy Castan
 * Graphics Library
 * CS250
 * Spring 2026
 */
#pragma once

#include "is_equal.h"

namespace CS250
{
    

    template <typename Element>
    constexpr bool Vector2<Element>::operator==(const Vector2<Element>& v) const noexcept
    {
        return is_equal(x, v.x) && is_equal(y, v.y);
    }
    template <typename Element>
    constexpr bool Vector2<Element>::operator!=(const Vector2<Element>& v) const noexcept
    {
        return !operator==(v);
    }

    template <typename Element>
    constexpr Vector2<Element> Vector2<Element>::operator+(const Vector2<Element>& v) const noexcept
    {
        return {x + v.x, y + v.y};
    }
    template <typename Element>
    constexpr Vector2<Element>& Vector2<Element>::operator+=(const Vector2<Element>& v) noexcept
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    template <typename Element>
    constexpr Vector2<Element> Vector2<Element>::operator-(const Vector2<Element>& v) const noexcept
    {
        return {x - v.x, y - v.y};
    }
    template <typename Element>
    constexpr Vector2<Element>& Vector2<Element>::operator-=(const Vector2<Element>& v) noexcept
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    template <typename Element>
    constexpr Vector2<Element> Vector2<Element>::operator-() const noexcept
    {
        return {-x, -y};
    }

    template <typename Element>
    constexpr Vector2<Element> Vector2<Element>::operator*(Element scale) const noexcept
    {
        return {x * scale, y * scale};
    }
    template <typename Element>
    constexpr Vector2<Element> Vector2<Element>::operator/(Element divisor) const noexcept
    {
        return {x / divisor, y / divisor};
    }
    template <typename Element>
    constexpr Vector2<Element>& Vector2<Element>::operator*=(Element scale) noexcept
    {
        x *= scale, y *= scale;
        return *this;
    }
    template <typename Element>
    constexpr Vector2<Element>& Vector2<Element>::operator/=(Element divisor) noexcept
    {
        x /= divisor, y /= divisor;
        return *this;
    }
    template <typename Element>
    constexpr Vector2<Element> operator*(Element scale, const Vector2<Element>& v) noexcept
    {
        return v * scale;
    }
}