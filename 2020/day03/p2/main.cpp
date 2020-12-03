#include <lib/FileUtil.h>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>

using map_t = std::vector<std::vector<char>>;

auto print_map(const map_t& map) -> void
{
    for(const auto& row : map)
    {
        for(const auto& col : row)
        {
            std::cout << col;
        }
        std::cout << "\n";
    }
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);

    map_t global_map{};

    for(const auto& line : chain::str::split(contents, '\n'))
    {
        auto& row = global_map.emplace_back();
        for(const auto& c : line)
        {
            row.emplace_back(c);
        }
    }

    std::vector<std::pair<uint64_t, uint64_t>> slopes{
        {1, 1},
        {3, 1},
        {5, 1},
        {7, 1},
        {1, 2}
    };

    uint64_t tree_product{1};

    for(const auto& [x_slope, y_slope] : slopes)
    {
        auto map = global_map;

        size_t trees_encountered{0};

        size_t y_pos{0};
        size_t x_pos{0};

        while(y_pos < map.size())
        {
            auto& point = map[y_pos][x_pos];
            if(point == '#')
            {
                point = 'X';
                ++trees_encountered;
            }
            else
            {
                point = 'O';
            }

            y_pos += y_slope;
            x_pos += x_slope;

            // Wrap around if overflowed.
            if(x_pos >= map[0].size())
            {
                x_pos -= map[0].size();
            }
        }

        print_map(map);
        std::cout << trees_encountered << " trees were encountered.\n";

        tree_product *= trees_encountered;
    }

    std::cout << "\ntree product = " << tree_product << "\n";

    return 0;
}
