#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <map>

#include <lib/FileUtil.h>
#include <chain/chain.hpp>

using Grid = std::array<char, 25>;
using World = std::map<int64_t, Grid>;

static const std::vector<
    std::vector<std::pair<int64_t, int64_t>>
> adj =
std::vector<
    std::vector<std::pair<int64_t, int64_t>>
>
{
    std::vector<std::pair<int64_t, int64_t>> // 1
    {
        std::pair{-1, 8},
        std::pair{0, 2},
        std::pair{0, 6},
        std::pair{-1, 12},
    },
    std::vector<std::pair<int64_t, int64_t>> // 2
    {
        std::pair{-1, 8},
        std::pair{0, 3},
        std::pair{0, 7},
        std::pair{0, 1},
    },
    std::vector<std::pair<int64_t, int64_t>> // 3
    {
        std::pair{-1, 8},
        std::pair{0, 4},
        std::pair{0, 8},
        std::pair{0, 2},
    },
    std::vector<std::pair<int64_t, int64_t>> // 4
    {
        std::pair{-1, 8},
        std::pair{0, 5},
        std::pair{0, 9},
        std::pair{0, 3},
    },
    std::vector<std::pair<int64_t, int64_t>> // 5
    {
        std::pair{-1, 8},
        std::pair{-1, 14},
        std::pair{0, 10},
        std::pair{0, 4},
    },
    std::vector<std::pair<int64_t, int64_t>> // 6
    {
        std::pair{0, 1},
        std::pair{0, 7},
        std::pair{0, 11},
        std::pair{-1, 12},
    },
    std::vector<std::pair<int64_t, int64_t>> // 7
    {
        std::pair{0, 2},
        std::pair{0, 8},
        std::pair{0, 12},
        std::pair{0, 6},
    },
    std::vector<std::pair<int64_t, int64_t>> // 8
    {
        std::pair{0, 3},
        std::pair{0, 9},
        std::pair{1, 5},
        std::pair{1, 4},
        std::pair{1, 3},
        std::pair{1, 2},
        std::pair{1, 1},
        std::pair{0, 7},
    },
    std::vector<std::pair<int64_t, int64_t>> // 9
    {
        std::pair{0, 4},
        std::pair{0, 10},
        std::pair{0, 14},
        std::pair{0, 8},
    },
    std::vector<std::pair<int64_t, int64_t>> // 10
    {
        std::pair{0, 5},
        std::pair{-1, 14},
        std::pair{0, 15},
        std::pair{0, 9},
    },
    std::vector<std::pair<int64_t, int64_t>> // 11
    {
        std::pair{0, 6},
        std::pair{0, 12},
        std::pair{0, 16},
        std::pair{-1, 12},
    },
    std::vector<std::pair<int64_t, int64_t>> // 12
    {
        std::pair{0, 7},
        std::pair{1, 1},
        std::pair{1, 6},
        std::pair{1, 11},
        std::pair{1, 16},
        std::pair{1, 21},
        std::pair{0, 17},
        std::pair{0, 11},
    },
    std::vector<std::pair<int64_t, int64_t>> // 13
    {
        // 13 is the portal up/down
    },
    std::vector<std::pair<int64_t, int64_t>> // 14
    {
        std::pair{0, 9},
        std::pair{0, 15},
        std::pair{0, 19},
        std::pair{1, 5},
        std::pair{1, 10},
        std::pair{1, 15},
        std::pair{1, 20},
        std::pair{1, 25},
    },
    std::vector<std::pair<int64_t, int64_t>> // 15
    {
        std::pair{0, 10},
        std::pair{-1, 14},
        std::pair{0, 20},
        std::pair{0, 14},
    },
    std::vector<std::pair<int64_t, int64_t>> // 16
    {
        std::pair{0, 11},
        std::pair{0, 17},
        std::pair{0, 21},
        std::pair{-1, 12},
    },
    std::vector<std::pair<int64_t, int64_t>> // 17
    {
        std::pair{0, 12},
        std::pair{0, 18},
        std::pair{0, 22},
        std::pair{0, 16},
    },
    std::vector<std::pair<int64_t, int64_t>> // 18
    {
        std::pair{1, 21},
        std::pair{1, 22},
        std::pair{1, 23},
        std::pair{1, 24},
        std::pair{1, 25},
        std::pair{0, 19},
        std::pair{0, 23},
        std::pair{0, 17},
    },
    std::vector<std::pair<int64_t, int64_t>> // 19
    {
        std::pair{0, 14},
        std::pair{0, 20},
        std::pair{0, 24},
        std::pair{0, 18},
    },
    std::vector<std::pair<int64_t, int64_t>> // 20
    {
        std::pair{0, 15},
        std::pair{-1, 14},
        std::pair{0, 25},
        std::pair{0, 19},
    },
    std::vector<std::pair<int64_t, int64_t>> // 21
    {
        std::pair{0, 16},
        std::pair{0, 22},
        std::pair{-1, 18},
        std::pair{-1, 12},
    },
    std::vector<std::pair<int64_t, int64_t>> // 22
    {
        std::pair{0, 17},
        std::pair{0, 23},
        std::pair{-1, 18},
        std::pair{0, 21},
    },
    std::vector<std::pair<int64_t, int64_t>> // 23
    {
        std::pair{0, 18},
        std::pair{0, 24},
        std::pair{-1, 18},
        std::pair{0, 22},
    },
    std::vector<std::pair<int64_t, int64_t>> // 24
    {
        std::pair{0, 19},
        std::pair{0, 25},
        std::pair{-1, 18},
        std::pair{0, 23},
    },
    std::vector<std::pair<int64_t, int64_t>> // 25
    {
        std::pair{0, 20},
        std::pair{-1, 14},
        std::pair{-1, 18},
        std::pair{0, 24},
    }
};

auto make_grid() -> Grid
{
    return Grid{
        '.','.','.','.','.',
        '.','.','.','.','.',
        '.','.','.','.','.',
        '.','.','.','.','.',
        '.','.','.','.','.'
    };
}

static const Grid EMPTY_GRID = make_grid();

auto evolve(
    World& world_now,
    World& world_future
) -> void
{
    for(auto& [depth, grid_now] : world_now)
    {
        if(auto found = world_future.find(depth); found == world_future.end())
        {
            world_future.emplace(depth, make_grid());
        }

        auto& grid_future = world_future[depth];
        
        for(int64_t i = 0; i < 25; ++i)
        {
            uint64_t bug_count{0};
            for(const auto& [depth_adj, pos] : adj[i])
            {
                if(auto found = world_now.find(depth + depth_adj); found != world_now.end())
                {
                    if(found->second[pos - 1] == '#')
                    {
                        ++bug_count;
                    }
                }
                else
                {
                    world_future.emplace(depth + depth_adj, make_grid());
                }
            }

            if(grid_now[i] == '.')
            {
                if(bug_count >= 1 && bug_count <= 2)
                {
                    grid_future[i] = '#';
                }
                else
                {
                    grid_future[i] = '.';
                }
            }
            else if(grid_now[i] == '#')
            {
                if(bug_count != 1)
                {
                    grid_future[i] = '.';
                }
                else
                {
                    grid_future[i] = '#';
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
            auto i = y * 5 + x;
            if(i == 12)
            {
                os << '?';
            }
            else
            {
                os << g[i];
            }
        }
        os << '\n';
    }
    return os;
}

auto operator<<(
    std::ostream& os,
    const World& w
) -> std::ostream&
{
    for(const auto& [depth, grid] : w)
    {
        // skip printing empty grids
        if(grid == EMPTY_GRID)
        {
            continue;
        }

        os << "Depth " << depth << '\n';
        os << grid << '\n';
    }
    return os;
}

auto count_bugs(
    World& world
) -> int64_t
{
    int64_t bug_count{0};
    for(const auto& [level, grid] : world)
    {
        for(std::size_t y = 0; y < 5; ++y)
        {
            for(std::size_t x = 0; x < 5; ++x)
            {
                if(grid[y * 5 + x] == '#')
                {
                    ++bug_count;
                }
            }
        }
    }
    return bug_count;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 3)
    {
        std::cout << args[0] << " <input_file> <minutes>" << std::endl;
        return 1;
    }

    auto minutes = std::stoul(args[2]);

    World world1{};
    World world2{};

    auto contents = file::read(args[1]);
    auto lines = chain::str::split(contents, '\n');

    for(std::size_t y = 0; y < 5; ++y)
    {
        for(std::size_t x = 0; x < 5; ++x)
        {
            world1[0][y * 5 + x] = lines[y][x];
        }
    }

    world1.emplace(1, make_grid());
    world1.emplace(-1, make_grid());

    for(uint64_t i = 0; i < minutes; ++i)
    {
        evolve(world1, world2);
        std::swap(world1, world2);
        world2.clear();
    }

    std::cout << world1;
    std::cout << "Number of bugs " << count_bugs(world1) << '\n';

    return 0;
}
