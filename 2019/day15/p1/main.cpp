#include <iostream>
#include <vector>
#include <thread>

#include <2019/intcode/Interpreter.hpp>

#include <lib/AStar.hpp>

enum class Movement
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

static std::map<Movement, Movement> right_hand {
    { Movement::NORTH, Movement::EAST },
    { Movement::WEST,  Movement::NORTH },
    { Movement::SOUTH, Movement::WEST },
    { Movement::EAST,  Movement::SOUTH}
};

static std::map<Movement, Movement> left_turn {
    { Movement::NORTH, Movement::WEST },
    { Movement::WEST,  Movement::SOUTH },
    { Movement::SOUTH, Movement::EAST },
    { Movement::EAST,  Movement::NORTH}
};

static std::map<Movement, std::pair<int64_t, int64_t>> movements {
    { Movement::NORTH, {-1, 0}},
    { Movement::WEST,  {0, -1}},
    { Movement::SOUTH, {+1, 0}},
    { Movement::EAST,  {0, +1}},
};

static std::map<Movement, Movement> reverse {
    { Movement::NORTH, Movement::SOUTH },
    { Movement::WEST, Movement::EAST },
    { Movement::SOUTH, Movement::NORTH },
    { Movement::EAST, Movement::WEST },
};

using Map = std::map<int64_t, std::map<int64_t, Tile>>;

auto reveal_tile(
    Map& map,
    int64_t d_x,
    int64_t d_y,
    Movement input,
    Tile tile
) -> void
{
    switch(input)
    {
        case Movement::WEST:
            map[d_x][d_y - 1] = tile;
        break;
        case Movement::SOUTH:
            map[d_x + 1][d_y] = tile;
        break;
        case Movement::EAST:
            map[d_x][d_y + 1] = tile;
        break;
        case Movement::NORTH:
            map[d_x - 1][d_y] = tile;
        break;
    }
}

struct Node;
using AStarMap = std::map<int64_t, std::map<int64_t, std::unique_ptr<Node>>>;
static AStarMap g_astar_map{};

auto print_map(Map& map, int64_t d_x, int64_t d_y) -> void;

struct Node : public astar::INode
{
    Node(
        intcode::Interpreter& _interpreter,
        Map& _map,
        int64_t _x,
        int64_t _y
    )
        : interpreter(_interpreter)
        , map(_map) 
        , x(_x)
        , y(_y)
        , tile(map[x][y])
    {
        
    }

    intcode::Interpreter& interpreter;
    Map& map;
    int64_t x{0};
    int64_t y{0};
    Tile tile{Tile::UNKNOWN};

    auto MovementCost(INode& parent) -> void override
    {
        movement_cost = parent.MovementCost() + 1;
    }
    auto EstimatedCost(INode& goal) -> void override
    {
        auto& goal_node = static_cast<Node&>(goal);
        estimated_cost = std::abs(x - goal_node.x) + std::abs(y - goal_node.y);
    }
    virtual auto Children() -> std::vector<INode*>& override
    {
        if(!children.has_value())
        {
            std::vector<INode*> neighbors{};

            for(const auto& [dir, coords] : movements)
            {
                auto [c_x, c_y] = coords;
                auto tile = map[x + c_x][y + c_y];

                // add all empty neighbors to the child search list.
                if(tile == Tile::EMPTY)
                {
                    if(g_astar_map[x + c_x][y + c_y] == nullptr)
                    {
                        g_astar_map[x + c_x][y + c_y] 
                            = std::make_unique<Node>(interpreter, map, x + c_x, y + c_y);
                    }

                    // std::cout << "Adding child [" << (x + c_x) << "," << (y + c_y) << "]\n";
                    neighbors.push_back(g_astar_map[x + c_x][y + c_y].get());
                }
            }

            children = std::move(neighbors);
        }
        
        return children.value();
    }
    auto IsGoal(INode& goal) const -> bool override
    {
        auto& goal_node = static_cast<Node&>(goal);
        return x == goal_node.x && y == goal_node.y;
    }
};

auto print_map(Map& map, int64_t d_x, int64_t d_y) -> void
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
            if(x == d_x && y == d_y)
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

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    intcode::Interpreter interpreter{args[1]};

    Map map{};

    // Droid coordinates and current direction its facing
    // for the maze right hand rule to follow the right wall.
    Movement d_m{Movement::NORTH};
    int64_t d_x{0};
    int64_t d_y{0};

    // oxygen station coordinates.
    int64_t o_x{0};
    int64_t o_y{0};

    bool found_oxygen_stations{false};
    while(!found_oxygen_stations)
    {
        map[d_x][d_y] = Tile::EMPTY; // any square the droid stands on must be empty

        print_map(map, d_x, d_y);

        bool moved{false};
        while(!moved)
        {
            Status s;

            std::cout << d_x << "," << d_y << " " << static_cast<int64_t>(d_m) << std::endl;

            // check tile to our right based on which direction we are facing
            auto right_check = right_hand[d_m];

            interpreter.Input(static_cast<int64_t>(right_check));
            interpreter.Execute();
            s = static_cast<Status>(interpreter.Output());

            switch(s)
            {
                case Status::WALL:
                    // Rotate left until we find an open area.
                    reveal_tile(map, d_x, d_y, right_check, Tile::WALL);
                    d_m = left_turn[d_m];
                break;
                case Status::MOVED:
                {
                    moved = true;
                    d_m = right_check;
                    const auto& [x, y] = movements[d_m];
                    d_x += x;
                    d_y += y;
                }
                break;
                case Status::OXYGEN_STATION:
                    found_oxygen_stations = true;
                    moved = true;
                    reveal_tile(map, d_x, d_y, right_check, Tile::OXYGEN_STATION);
                    std::cout << "Found oxygen station [" << d_x << "," << d_y << "]" << std::endl;

                    // the oxygen coordinates for the astar algo need to start
                    // exactly where the droid left off, not on the oxygen station
                    // itself
                    o_x = d_x;
                    o_y = d_y;
                break;
            }
        }
    }

    print_map(map, d_x, d_y);

    for(const auto& [x, row] : map)
    {
        for(const auto& [y, col] : row)
        {
            if(col == Tile::OXYGEN_STATION)
            {
                std::cout << "[" << x << "," << y << "] is the oxygen station.";
                std::cout << std::endl;
                g_astar_map[x][y] = std::make_unique<Node>(interpreter, map, x, y);
            }
        }
    }

    g_astar_map[o_x][o_y] = std::make_unique<Node>(interpreter, map, o_x, o_y);
    g_astar_map[0][0] = std::make_unique<Node>(interpreter, map, 0, 0);

    astar::AStar a{
        *g_astar_map[o_x][o_y].get(),
        *g_astar_map[0][0].get()
    };

    uint64_t iterations{0};
    while(true)
    {
        auto exec_state = a.Step();
        switch(exec_state)
        {
            case astar::State::FAILED:
                std::cout << "A* failed :(" << std::endl;
                return 0;
            break;
            case astar::State::SEARCHING:
                // std::cout << "Searching iteration " << iterations << "\n";
                // std::cout << "OpenList.size() == " << a.OpenList().size() << "\n";
                // std::cout << "ClosedList.size() == " << a.ClosedList().size() << "\n";
                // std::cout << std::endl;
            break;
            case astar::State::GOALFOUND:
            {
                auto path = a.Path();
                Map path_map{};
                for(const auto& inode : path)
                {
                    auto* node = static_cast<const Node*>(inode);

                    path_map[node->x][node->y] = Tile::EMPTY;
                }

                print_map(path_map, 0, 0);

                std::cout << "Steps: " << path.size() << std::endl;
                return 0;
            }
            break;
        }

        ++iterations;
    }

    return 0;
}
