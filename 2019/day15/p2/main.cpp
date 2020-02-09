#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <optional>
#include <iomanip>

#include <chrono>
#include <thread>

#include <2019/intcode/Interpreter.hpp>

enum class Direction
{
    NORTH = 1,
    SOUTH = 2,
    WEST = 3,
    EAST = 4
};

enum class Tile
{
    UNKNOWN = 0,
    EMPTY = 1,
    WALL = 2,
    OXYGEN_STATION = 3
};

enum class Status
{
    WALL = 0,
    MOVED = 1,
    OXYGEN_STATION = 2
};

static std::map<Direction, std::pair<int64_t, int64_t>> g_movements {
    { Direction::NORTH, {-1, 0}},
    { Direction::WEST,  {0, -1}},
    { Direction::SOUTH, {+1, 0}},
    { Direction::EAST,  {0, +1}},
};

static std::map<Direction, Direction> g_reverse {
    { Direction::NORTH, Direction::SOUTH },
    { Direction::WEST, Direction::EAST },
    { Direction::SOUTH, Direction::NORTH },
    { Direction::EAST, Direction::WEST },
};

static std::vector<Direction> g_directions {
    Direction::NORTH,
    Direction::EAST,
    Direction::SOUTH,
    Direction::WEST
};

using Map = std::map<int64_t, std::map<int64_t, Tile>>;
using DepthMap = std::map<int64_t, std::map<int64_t, std::optional<uint64_t>>>;

struct Point
{
    int64_t x{0};
    int64_t y{0};
};

auto print_map(
    Map& map,
    Point droid
) -> void
{
    int64_t min_x{0};
    int64_t max_x{0};   
    int64_t min_y{0};
    int64_t max_y{0};
    for(const auto& [x, row] : map)
    {
        min_x = std::min(min_x, x);
        max_x = std::max(max_x, x);

        for(const auto& [y, col] : row)
        {
            min_y = std::min(min_y, y);
            max_y = std::max(max_y, y);
        }
    }

    for(int64_t x = min_x; x <= max_x; ++x)
    {
        for(int64_t y = min_y; y <= max_y; ++y)
        {
            if(x == droid.x && y == droid.y)
            {
                std::cout << 'D';
                continue;
            }

            if(x == 0 && y == 0)
            {
                std::cout << 'S';
                continue;
            }

            switch(map[x][y])
            {
                case Tile::UNKNOWN:
                    std::cout << ' ';
                    break;
                case Tile::EMPTY:
                    std::cout << '.';
                    break;
                case Tile::WALL:
                    std::cout << '#';
                    break;
                case Tile::OXYGEN_STATION:
                    std::cout << "@";
                    break;
            }
        }
        std::cout << '\n';
    }

    std::cout << "\n" << std::endl;
}

auto print_depth_map(
    DepthMap& map
) -> void
{
    int64_t min_x{0};
    int64_t max_x{0};   
    int64_t min_y{0};
    int64_t max_y{0};
    for(const auto& [x, row] : map)
    {
        min_x = std::min(min_x, x);
        max_x = std::max(max_x, x);

        for(const auto& [y, col] : row)
        {
            min_y = std::min(min_y, y);
            max_y = std::max(max_y, y);
        }
    }

    for(int64_t x = min_x; x <= max_x; ++x)
    {
        for(int64_t y = min_y; y <= max_y; ++y)
        {
            auto& v = map[x][y];
            if(v.has_value())
            {
                std::cout << std::setw(3) << v.value() << ',';
            }
            else
            {
                std::cout << "    ";
            }
        }
        std::cout << '\n';
    }

    std::cout << "\n" << std::endl;
}

auto explore(
    intcode::Interpreter interpreter,
    Map& map
) -> void
{
    std::queue<std::pair<intcode::Interpreter, Point>> q{};

    q.emplace(interpreter, Point{0, 0});
    map[0][0] = Tile::EMPTY;

    while(!q.empty())
    {
        for(const auto dir : g_directions)
        {
            // This is super inefficient but it saves the interperters 'state'
            // at each tile so the algo never has to 'undo' steps.
            auto [i, p] = q.front();
            auto [x, y] = g_movements[dir];
            x += p.x;
            y += p.y;

            if(map[x][y] == Tile::UNKNOWN)
            {
                i.Input(static_cast<int64_t>(dir));
                i.Execute();
                auto status = static_cast<Status>(i.Output());

                switch(status)
                {
                    case Status::MOVED:
                    {
                        map[x][y] = Tile::EMPTY;
                        q.emplace(std::move(i), Point{x, y});
                    }
                    break;
                    case Status::OXYGEN_STATION:
                        map[x][y] = Tile::OXYGEN_STATION;
                    break;
                    case Status::WALL:
                        map[x][y] = Tile::WALL;
                    break;
                }
            }
        }

        q.pop();
    }
}

auto fill_oxygen(
    Map& map,
    DepthMap& depth_map,
    Point oxygen_pos,
    uint64_t depth
) -> uint64_t
{
    std::queue<std::pair<Point, uint64_t>> q{};

    q.emplace(oxygen_pos, depth);
    depth_map[oxygen_pos.x][oxygen_pos.y] = 0;

    auto max_depth = depth;

    while(!q.empty())
    {
        auto [p, d] = q.front();
        q.pop();

        max_depth = std::max(max_depth, d);

        for(const auto dir : g_directions)
        {
            auto [x, y] = g_movements[dir];
            x += p.x;
            y += p.y;

            if(map[x][y] == Tile::WALL)
            {
                continue;
            }

            // Skip visited tiles.
            if(depth_map[x][y].has_value())
            {
                continue;
            }
            depth_map[x][y] = d + 1;

            // If we haven't visited this tile and its empty, enqueue.
            if(map[x][y] == Tile::EMPTY)
            {
                q.emplace(Point{x, y}, d + 1);
            }
        }
    }

    return max_depth;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    // use depth-first to explore the entire map
    // record parent direction to undo movements
    // to move back up the tree
    // 
    // once fully explored use breadth first search
    // to expand outwards from the oxygen station
    // to determine how many ticks it takes to reach 
    // the final leaf node, this is how many
    // minutes it takes to fill the rooms with oxygen

    intcode::Interpreter interpreter{args[1]};

    Map map{};
    explore(interpreter, map);

    print_map(map, Point{0, 0});

    Point oxygen_pos{0, 0};

    for(const auto& [x, row] : map)
    {
        for(const auto& [y, col] : row)
        {
            if(col == Tile::OXYGEN_STATION)
            {
                std::cout << "[" << x << "," << y << "] is the oxygen station.\n";
                oxygen_pos.x = x;
                oxygen_pos.y = y;
            }
        }
    }

    print_map(map, oxygen_pos);

    DepthMap depth_map{};
    map[oxygen_pos.x][oxygen_pos.y] = Tile::EMPTY;
    auto time_to_fill = fill_oxygen(map, depth_map, oxygen_pos, 0);
    map[oxygen_pos.x][oxygen_pos.y] = Tile::OXYGEN_STATION;

    print_depth_map(depth_map);

    std::cout << "Minutes to fill with oxygen: " << time_to_fill << std::endl;

    return 0;
}
