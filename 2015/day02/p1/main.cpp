#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <algorithm>

#include <chain/Chain.hpp>

int start(std::vector<std::string> args)
{
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream input_file{args[1]};

    uint64_t total_area = {0};

    std::string line{};
    while(std::getline(input_file, line))
    {
        auto parts = chain::str::split(line, std::string_view{"x"});
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

int main(int argc, char* argv[])
{
    return start({argv, argv + argc});
}
