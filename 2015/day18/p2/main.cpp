#include <iostream>
#include <vector>
#include <string>
#include <vector>
#include <array>

#include <lib/FileUtil.h>
#include <chain/Chain.hpp>

using Grid = std::vector<std::vector<char>>;

auto operator<<(std::ostream& os, const Grid& grid) -> std::ostream&
{
    for(const auto& row : grid)
    {
        for(const auto c : row)
        {
            std::cout << c;
        }
        std::cout << '\n';
    }

    return os;
}

static const std::array<std::pair<std::size_t, std::size_t>, 8> neighbors
{
    std::pair{-1, -1},
    std::pair{ 0, -1},
    std::pair{ 1, -1},
    std::pair{ 1,  0},
    std::pair{ 1,  1},
    std::pair{ 0,  1},
    std::pair{-1,  1},
    std::pair{-1,  0}
};

auto turn_on_corners(
    Grid& grid
) -> void
{
    grid[0][0] = '#';
    grid[0][grid.front().size() - 1] = '#';
    grid[grid.size() - 1][0] = '#';
    grid[grid.size() - 1][grid.front().size() - 1] = '#';
}

auto evolve(
    Grid& grid
) -> void
{
    Grid next = grid;

    for(std::size_t x = 0; x < grid.size(); ++x)
    {
        for(std::size_t y = 0; y < grid.front().size(); ++y)
        {
            uint64_t neighbors_on{0};

            for(const auto [x1, y1] : neighbors)
            {
                auto x2 = x + x1;
                auto y2 = y + y1;

                if(     x2 >= 0 && x2 < grid.size()
                    &&  y2 >= 0 && y2 < grid.front().size()
                )
                {
                    if(grid[x2][y2] == '#')
                    {
                        ++neighbors_on;
                    }
                }
            }

            if(grid[x][y] == '#')
            {
                // A light stays on when it has 2 or 3 neighbors on.
                if(neighbors_on != 2 && neighbors_on != 3)
                {
                    next[x][y] = '.';
                }
            }
            else
            {
                // A light turns on if 3 neighbors are on.
                if(neighbors_on == 3)
                {
                    next[x][y] = '#';
                }
            }
        }
    }

    turn_on_corners(next);

    grid.swap(next);
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 3)
    {
        std::cout << args[0] << " <input_file> <steps>" << std::endl;
        return 1;
    }

    auto steps = std::stoul(args[2]);

    auto contents = file::read(args[1]);
    auto lines = chain::str::split(contents, '\n');

    Grid grid{};

    for(const auto& line : lines)
    {
        grid.push_back({});
        for(auto c : line)
        {
            grid.back().push_back(c);
        }
    }

    turn_on_corners(grid);

    std::cout << "Initial state:\n";
    std::cout << grid << "\n\n";

    for(auto step = 0; step < steps; ++step)
    {
        evolve(grid);
        std::cout << "After " << step << " steps:\n";
        std::cout << grid << "\n\n";
    }

    uint64_t lights_on{0};
    for(const auto& row : grid)
    {
        for(const auto c : row)
        {
            if(c == '#') ++lights_on;
        }
    }

    std::cout << "Lights on = " << lights_on << '\n';

    return 0;
}
