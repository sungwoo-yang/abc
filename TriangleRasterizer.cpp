/*
 * Rudy Castan
 * Graphics Library
 * CS250
 * Spring 2026
 */

#include "Image.hpp"
#include "draw_triangle.h"

#include <array>
#include <iostream>

using namespace CS250;

constexpr int   WIDTH  = 256;
constexpr int   HEIGHT = 256;
constexpr Color BEAUTY_BUSH_PINK{0xf0bcd4FF};
constexpr Color DISCO_PURPLE{0x8a1c7cFF};
constexpr Color CABARET_RED{0xda4167FF};
constexpr Color JAGUAR_BLACK{0x1F0322FF};
constexpr Color WHITE{0xFFFFFFFF};
Image           gImage{WIDTH, HEIGHT, BEAUTY_BUSH_PINK};

using Point = vec2;

namespace text
{
    int main(void);
}

int main(void)
{
    Point p0(10, 10);
    Point p1(128, 10);
    Point p2(246, 246);
    Point p3(10, 246);
    Point p4(246, 10);
    Point p5(128, 246);

    const auto setpixel = [&](int column, int row, auto intensity) { gImage.SetPixel(column, row, intensity); };

    draw_triangle(p0, p1, p3, CABARET_RED, setpixel);
    draw_triangle(p1, p2, p3, DISCO_PURPLE, setpixel);
    draw_triangle(p4, p2, p1, JAGUAR_BLACK, setpixel);
    draw_triangle(p0, p5, p4, WHITE, setpixel);
    gImage.SaveToPNG("results.png");

    return text::main();
}

namespace text
{
    template <int W, int H>
    struct Image : std::array<char, W * H>
    {
        static constexpr int Width  = W;
        static constexpr int Height = H;
    };

    template <int W, int H>
    void print_image(const Image<W, H>& image);

    constexpr Image<16, 8> Test01() noexcept
    {
        using IMG = Image<16, 8>;
        IMG image{};
        for (auto& c : image)
            c = '.';

        const Point p0(0, 0);
        const Point p1(8, 0);
        const Point p2(15, 7);
        const Point p3(0, 7);
        const Point p4(15, 0);
        const Point p5(8, 7);

        const auto setpixel = [&](int column, int row, char value) constexpr
        {
            image[row * IMG::Width + column] = value;
        };

        draw_triangle(p0, p1, p3, '@', setpixel);
        draw_triangle(p1, p2, p3, '#', setpixel);
        draw_triangle(p4, p2, p1, 'O', setpixel);
        draw_triangle(p0, p5, p4, ' ', setpixel);
        return image;
    }

    constexpr Image<8, 8> Test02() noexcept
    {
        using IMG = Image<8, 8>;
        IMG image{};
        for (auto& c : image)
            c = '.';

        const Point p0(0.5f, 0.5f);
        const Point p1(7.5f, 0.5f);
        const Point p2(7.5f, 7.5f);
        const Point p3(0.5f, 7.5f);

        const auto setpixel = [&](int column, int row, char value) constexpr
        {
            image[row * IMG::Width + column] = value;
        };

        draw_triangle(p0, p1, p3, '#', setpixel);
        draw_triangle(p1, p2, p3, 'O', setpixel);
        return image;
    }

    constexpr Image<8, 8> Test03() noexcept
    {
        using IMG = Image<8, 8>;
        IMG image{};

        const Point p0(0.5f, 0.5f);
        const Point p1(7.5f, 0.5f);
        const Point p2(7.5f, 7.5f);
        const Point p3(0.5f, 7.5f);

        const auto setpixel = [&](int column, int row, char value) constexpr
        {
            image[row * IMG::Width + column] += value;
        };

        draw_triangle(p0, p1, p3, '\1', setpixel);
        draw_triangle(p1, p2, p3, '\1', setpixel);
        return image;
    }

    constexpr Image<8, 8> Test04() noexcept
    {
        using IMG = Image<8, 8>;
        IMG image{};
        for (auto& c : image)
            c = '.';

        const Point p0(4.0f, 1.0f);
        const Point p1(8.0f, 4.5f);
        const Point p2(4.0f, 7.5f);
        const Point p3(0.0f, 4.5f);

        const auto setpixel = [&](int column, int row, char value) constexpr
        {
            image[row * IMG::Width + column] = value;
        };

        draw_triangle(p0, p1, p3, '#', setpixel);
        draw_triangle(p1, p2, p3, 'O', setpixel);
        return image;
    }

    constexpr Image<8, 8> Test05() noexcept
    {
        using IMG = Image<8, 8>;
        IMG image{};

        const Point p0(4.0f, 1.0f);
        const Point p1(8.0f, 4.5f);
        const Point p2(4.0f, 7.5f);
        const Point p3(0.0f, 4.5f);

        const auto setpixel = [&](int column, int row, char value) constexpr
        {
            image[row * IMG::Width + column] += value;
        };

        draw_triangle(p0, p1, p3, '\1', setpixel);
        draw_triangle(p1, p2, p3, '\1', setpixel);
        return image;
    }

    void print_numbers(const Image<8, 8>& image);

    int main(void)
    {
        constexpr Image<16, 8> image01 = Test01();
        print_image(image01);

        std::cout << '\n';

        constexpr Image<8, 8> image02 = Test02();
        print_image(image02);

        std::cout << '\n';

        constexpr Image<8, 8> image03 = Test03();
        print_numbers(image03);

        std::cout << '\n';

        constexpr Image<8, 8> image04 = Test04();
        print_image(image04);

        std::cout << '\n';

        constexpr Image<8, 8> image05 = Test05();
        print_numbers(image05);

        return 0;
    }

    template <int W, int H>
    void print_image(const Image<W, H>& image)
    {
        for (int row = H - 1; row >= 0; --row)
        {
            for (int column = 0; column < W; ++column)
            {
                std::cout << image[row * W + column];
            }
            std::cout << '\n';
        }
    }

    void print_numbers(const Image<8, 8>& image)
    {
        using IMG = Image<8, 8>;
        for (int row = IMG::Height - 1; row >= 0; --row)
        {
            for (int column = 0; column < IMG::Width; ++column)
            {
                std::cout << static_cast<int>(image[row * IMG::Width + column]);
            }
            std::cout << '\n';
        }
    }
}