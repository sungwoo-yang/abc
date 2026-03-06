/*
 * Rudy Castan
 * Graphics Library
 * CS250
 * Spring 2026
 */
#pragma once

namespace CS250
{
    template <typename Element>
    struct [[nodiscard]] Vector2;

    using vec2  = Vector2<float>;
    using dvec2 = Vector2<double>;
    using ivec2 = Vector2<int>;

    template <typename Element>
    struct [[nodiscard]] Vector2
    {
        Element x{0.0};
        Element y{0.0};

        constexpr Vector2() noexcept = default;
        constexpr Vector2(Element fill_value) noexcept : Vector2{fill_value, fill_value} {}
        constexpr Vector2(Element x_value, Element y_value) noexcept : x{x_value}, y{y_value} {};

        [[nodiscard]] constexpr bool operator==(const Vector2& v) const noexcept;
        [[nodiscard]] constexpr bool operator!=(const Vector2& v) const noexcept;

        constexpr Vector2  operator+(const Vector2& v) const noexcept;
        constexpr Vector2& operator+=(const Vector2& v) noexcept;

        constexpr Vector2  operator-(const Vector2& v) const noexcept;
        constexpr Vector2& operator-=(const Vector2& v) noexcept;

        constexpr Vector2 operator-() const noexcept;

        constexpr Vector2  operator*(Element scale) const noexcept;
        constexpr Vector2  operator/(Element divisor) const noexcept;
        constexpr Vector2& operator*=(Element scale) noexcept;
        constexpr Vector2& operator/=(Element divisor) noexcept;
    };
    template <typename Element>
    constexpr Vector2<Element> operator*(Element scale, const Vector2<Element>& v) noexcept;
}

#include "vec2.inl"