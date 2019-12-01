#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <algorithm>

#include <sstream>

#include "../../lib/StringUtil.h"

int main(int argc, char* argv[])
{
    std::ifstream input_file("input.txt");

    uint64_t total_area = {0};

    // std::istringstream input_file;
    // input_file.str("3x11x24");

    std::string line{};
    while(std::getline(input_file, line))
    {
        auto parts = str::split(line, std::string_view{"x"});
        if(parts.size() != 3)
        {
            std::cerr << "malformed line " << line << std::endl;
            continue;
        }

        uint64_t l = std::stoul(std::string{parts[0]});
        uint64_t w = std::stoul(std::string{parts[1]});
        uint64_t h = std::stoul(std::string{parts[2]});

        auto lw = l * w;
        auto wh = w * h;
        auto hl = h * l;

        total_area += 2 * (lw + wh + hl);

        total_area += std::min(lw, std::min(wh, hl));
    }

    std::cout << total_area << std::endl;

    return 0;
}
