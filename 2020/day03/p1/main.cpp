#include <lib/file_util.hpp>
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

    map_t map{};

    for(const auto& line : chain::str::split(contents, '\n'))
    {
        auto& row = map.emplace_back();
        for(const auto& c : line)
        {
            row.emplace_back(c);
        }
    }

    size_t trees{0};

    size_t y_pos{0};
    size_t x_pos{0};

    while(y_pos < map.size())
    {
        auto& point = map[y_pos][x_pos];
        if(point == '#')
        {
            point = 'X';
            ++trees;
        }
        else
        {
            point = 'O';
        }

        y_pos += 1;
        x_pos = (x_pos + 3) % map[0].size();
    }

    print_map(map);
    std::cout << "\n" << trees << " trees were encountered.\n";

    return 0;
}
