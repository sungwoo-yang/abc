/*
 * Rudy Castan
 * Graphics Library
 * CS250
 * Spring 2026
 */
#pragma once

#include <limits>
#include <type_traits>

namespace CS250
{
    template <typename Element>
    using BoolForIntType = std::enable_if_t<std::is_integral_v<Element>, bool>;
    template <typename Element>
    using BoolForFloatType = std::enable_if_t<std::is_floating_point_v<Element>, bool>;


    template <typename Element>
    [[nodiscard]] constexpr BoolForIntType<Element> is_equal(Element d1, Element d2) noexcept
    {
        return d1 == d2;
    }

    template <typename Element>
    [[nodiscard]] constexpr Element absolute_value(Element d) noexcept
    {
        return (d < 0) ? -d : d;
    }

    template <typename Element>
    [[nodiscard]] constexpr BoolForFloatType<Element> is_equal(Element d1, Element d2) noexcept
    {
        return absolute_value(d1 - d2) <= std::numeric_limits<Element>::epsilon() * absolute_value(d1 + d2);
    }

    template <typename Element>
    [[nodiscard]] constexpr BoolForIntType<Element> is_zero(Element v) noexcept
    {
        return v == 0;
    }

    template <typename Element>
    [[nodiscard]] constexpr BoolForFloatType<Element> is_zero(Element v) noexcept
    {
        return absolute_value(v) <= std::numeric_limits<Element>::epsilon();
    }
}