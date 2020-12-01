#include <iostream>
#include <fstream>
#include <unordered_map>
#include <functional>
#include <vector>
#include <string>

#include <chain/chain.hpp>

struct Point
{
    bool wire1 = false;
    bool wire2 = false;
};

using GridType = 
    std::unordered_map<
        int64_t,
        std::unordered_map<int64_t, Point>
    >;

auto execute_commands(
    GridType& grid,
    const std::vector<std::string_view>& cmds,
    std::function<void(Point& p)> callback
) -> void
{
    int64_t x = 0;
    int64_t y = 0;
    for(const auto& cmd : cmds)
    {
        auto amount_view = cmd.substr(1);
        auto amount = std::stol(std::string{amount_view});

        for(std::size_t i = 0; i < amount; ++i)
        {
            switch(cmd[0])
            {
                case 'R':
                    ++x;
                break;
                case 'L':
                    --x;
                break;
                case 'U':
                    ++y;
                break;
                case 'D':
                    --y;
                break;
                default:
                    std::cerr << "Unknown command: " << cmd << std::endl;
                    std::exit(-1);
            }

            callback(grid[x][y]);
        }
    }
}

int start(std::vector<std::string> args)
{
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::fstream input_file{args[1]};

    std::string raw_wire1_cmds{};
    std::string raw_wire2_cmds{};

    std::getline(input_file, raw_wire1_cmds);
    std::getline(input_file, raw_wire2_cmds);

    auto wire1_cmds = chain::str::split(raw_wire1_cmds, ',');
    auto wire2_cmds = chain::str::split(raw_wire2_cmds, ',');

    GridType grid{};

    execute_commands(grid, wire1_cmds, [](Point& p) { p.wire1 = true; });
    execute_commands(grid, wire2_cmds, [](Point& p) { p.wire2 = true; });

    int64_t min_manhattan_distance = std::numeric_limits<int64_t>::max();

    for(const auto& [x, ygrid] : grid)
    {
        for(const auto& [y, point] : ygrid)
        {
            if(point.wire1 && point.wire2)
            {
                min_manhattan_distance = std::min(
                    min_manhattan_distance,
                    std::abs(x) + std::abs(y));
            }
        }
    }

    std::cout << min_manhattan_distance << std::endl;

    return 0;
}

int main(int argc, char* argv[])
{
    return start({argv, argv + argc});
}
