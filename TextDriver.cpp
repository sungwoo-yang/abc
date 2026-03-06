#include "draw_triangle.h"

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace CS250;

constexpr int WIDTH  = 16;
constexpr int HEIGHT = 8;

struct TestCase
{
    float                            x0{0}, y0{0}, x1{0}, y1{0}, x2{0}, y2{0};
    std::array<char, WIDTH * HEIGHT> result{};
};

std::string to_string(const TestCase& test_case)
{
    std::stringstream ss;
    ss << "Draw Triangle (" << test_case.x0 << "," << test_case.y0 << ") (" << test_case.x1 << "," << test_case.y1
       << ") (" << test_case.x2 << "," << test_case.y2 << ")";
    return ss.str();
}

template <typename ARRAY>
void print_image(const ARRAY& image)
{
    for (int row = HEIGHT - 1; row >= 0; --row)
    {
        for (int column = 0; column < WIDTH; ++column)
        {
            std::cout << image[row * WIDTH + column];
        }
        std::cout << '\n';
    }
}


int main(void)
{
    ifstream file{"test_cases.txt"};
    int      max_visuals_report = 10;
    long     numCasesPassed     = 0;
    long     numCases           = 0;
    TestCase test_case;
    while (file >> test_case.x0 >> test_case.y0 >> test_case.x1 >> test_case.y1 >> test_case.x2 >> test_case.y2)
    {
        ++numCases;
        for (auto& val : test_case.result)
        {
            file >> val;
        }

        std::array<char, WIDTH * HEIGHT> image{};
        image.fill('.');
        CS250::draw_triangle({test_case.x0, test_case.y0}, {test_case.x1, test_case.y1}, {test_case.x2, test_case.y2},
                             '#', [&](int x, int y, char c) {
                                 const bool column_is_good = (x >= 0 && x < WIDTH);
                                 const bool row_is_good    = (y >= 0 && y < HEIGHT);

                                 if (column_is_good && row_is_good)
                                 {
                                     image[y * WIDTH + x] = c;
                                 }
                                 else
                                 {
                                     std::cerr << "Failed Test Case - " << to_string(test_case) << '\n';
                                     std::cerr << "BAD SETPIXEL VALUE: (" << x << ", " << y << ")\n";
                                     std::exit(-1);
                                 }
                             });
        if (image != test_case.result)
        {
            if (max_visuals_report > 0)
            {
                std::cout << " Failed Test Case - " << to_string(test_case) << '\n';

                std::cout << "---------------\n"
                          << "Expected\n"
                          << "---------------\n";
                print_image(test_case.result);

                std::cout << "---------------\n"
                          << "Student Produced\n"
                          << "---------------\n";
                print_image(image);
                --max_visuals_report;
            }
        }
        else
        {
            ++numCasesPassed;
        }
    }
    std::cout << " Test Cases passed " << numCasesPassed << "/" << numCases << '\n';

    return 0;
}