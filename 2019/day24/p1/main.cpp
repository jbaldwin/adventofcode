#include <iostream>
#include <vector>
#include <string>
#include <array>

#include <lib/FileUtil.h>
#include <lib/StringUtil.h>

using Grid = std::array<std::array<char, 5>, 5>;

auto evolve(
    const Grid& now,
    Grid& future
) -> void
{
    static const std::array<std::pair<int64_t, int64_t>, 4> adj =
    {
        std::pair{ 1,  0},
        std::pair{-1,  0},
        std::pair{ 0,  1},
        std::pair{ 0, -1}
    };

    for(std::size_t y = 0; y < 5; ++y)
    {
        for(std::size_t x = 0; x < 5; ++x)
        {
            future[y][x] = now[y][x];

            int64_t bug_count{0};

            for(const auto& [x1, y1] : adj)
            {
                try
                {
                    if(now.at(y + y1).at(x + x1) == '#')
                    {
                        ++bug_count;
                    }
                }
                catch(...) { }
            }

            if(now[y][x] == '.')
            {
                if(bug_count >= 1 && bug_count <= 2)
                {
                    future[y][x] = '#';
                }
            }
            else if(now[y][x] == '#')
            {
                if(bug_count != 1)
                {
                    future[y][x] = '.';
                }
            }
        }
    }
}

auto operator<<(
    std::ostream& os,
    const Grid& g
) -> std::ostream&
{
    for(std::size_t y = 0; y < 5; ++y)
    {
        for(std::size_t x = 0; x < 5; ++x)
        {
            os << g[y][x];
        }
        os << '\n';
    }
}

auto calculate_biodiversity(
    const Grid& grid
) -> uint64_t
{
    uint64_t biodiversity{0};
    uint64_t counter{1};
    for(std::size_t y = 0; y < 5; ++y)
    {
        for(std::size_t x = 0; x < 5; ++x)
        {
            if(grid[y][x] == '#')
            {
                biodiversity += counter;
            }

            counter *= 2;
        }   
    }

    return biodiversity;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    Grid grid{};

    auto contents = file::read(args[1]);
    auto lines = str::split(contents, '\n');

    for(std::size_t y = 0; y < 5; ++y)
    {
        for(std::size_t x = 0; x < 5; ++x)
        {
            grid[y][x] = lines[y][x];
        }
    }

    std::vector<Grid> grids;
    grids.push_back(grid);

    bool done{false};
    while(!done)
    {
        Grid future{};
        evolve(grids.back(), future);

        std::cout << future << '\n';

        for(const auto& g : grids)
        {
            if(future == g)
            {
                std::cout << "Found duplicate at " << (grids.size() + 1) << '\n';
                done = true;
                break;
            }
        }

        grids.push_back(std::move(future));
    }

    auto rating = calculate_biodiversity(grids.back());
    std::cout << "Biodiversity rating = " << rating << '\n';

    return 0;
}
