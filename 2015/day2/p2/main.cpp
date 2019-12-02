#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <algorithm>

#include "../../../lib/StringUtil.h"

int main(int argc, char* argv[])
{
    std::ifstream input_file("../p1/input.txt");

    uint64_t total_ribbon = {0};

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

        uint64_t ss1 = {0};
        uint64_t ss2 = {0};

         if(l <= h && w <= h)
        {
            ss1 = l;
            ss2 = w;
        }
        else if (l <= w && h <= w)
        {
            ss1 = l;
            ss2 = h;
        }
        else
        {
            ss1 = h;
            ss2 = w;
        }

        total_ribbon += 2 * ss1 + 2 * ss2;
        total_ribbon += l * w * h;
    }

    std::cout << total_ribbon << std::endl;

return 0;
}