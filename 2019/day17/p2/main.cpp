#include <iostream>
#include <vector>
#include <queue>
#include <tuple>
#include <chrono>
#include <thread>
#include <variant>

#include <2019/intcode/Interpreter.hpp>

struct Point
{
    int64_t x{0};
    int64_t y{0};
};

enum class Turn
{
    STRAIGHT,
    LEFT,
    RIGHT
};

enum class Direction
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct Tile
{
    // The tiles true type.
    char c{'.'};
    // Has this tile been visited while generating instructions?
    bool visited{false};
    // Interesections that have been visited can be visited again.
    bool is_intersection{false};
};

using Map = std::vector<std::vector<Tile>>;

auto print_map(
    Map& map,
    const Point& droid,
    Direction dir,
    bool falling
) -> void
{
    std::cout << "Droid position [" << droid.x << ',' << droid.y << "]\n";
    std::cout << "Direction ";
    switch(dir)
    {
        case Direction::UP: std::cout << "UP\n"; break;
        case Direction::DOWN: std::cout << "DOWN\n"; break;
        case Direction::LEFT: std::cout << "LEFT\n"; break;
        case Direction::RIGHT: std::cout << "RIGHT\n"; break;
    }
    std::cout << "Failling " << ((falling) ? "yes\n" : "no\n");

    // y is down
    for(int64_t y = 0; y < map.size(); ++y)
    {
        // x is right
        for(int64_t x = 0; x < map[y].size(); ++x)
        {
            auto& tile = map[y][x];

            if(x == droid.x && y == droid.y)
            {
                if(falling)
                {
                    std::cout << 'X';
                    continue;
                }

                switch(dir)
                {
                    case Direction::LEFT: 
                        std::cout << '<';
                    break;
                    case Direction::RIGHT: 
                        std::cout << '>';
                    break;
                    case Direction::UP: 
                        std::cout << '^';
                    break;
                    case Direction::DOWN:
                        std::cout << 'v';
                    break;
                }
            }
            else
            {
                std::cout << tile.c;
            }
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
}

auto find_intersections(
    Map& map
) -> void
{
    for(std::size_t x = 0; x < map.size(); ++x)
    {
        for(std::size_t y = 0; y < map[x].size(); ++y)
        {
            if(map[x][y].c == '#')
            {
                try
                {
                    if(    map.at(x + 1).at(y).c == '#'
                        && map.at(x - 1).at(y).c == '#'
                        && map.at(x).at(y + 1).c == '#'
                        && map.at(x).at(y - 1).c == '#'
                    )
                    {
                        map[x][y].is_intersection = true;
                    }
                }
                catch(...)
                {
                    // .. ignore at boundary
                }
            }
        }
    }
}

auto instructions_collapse(
    std::vector<std::tuple<Point, Direction, Turn>> input
) -> std::vector<std::variant<int64_t, Turn>>
{
    std::vector<std::variant<int64_t, Turn>> output{};

    bool first{true};
    for(const auto& value : input)
    {
        // Skip the first instruction since its a starting point to generate instructions.
        if(first)
        {
            first = false;
            continue;
        }

        const auto& turn = std::get<Turn>(value);

        switch(turn)
        {
            case Turn::STRAIGHT:
            {
                if(output.empty())
                {
                    output.push_back({1});
                }
                else
                {
                    if(std::holds_alternative<int64_t>(output.back()))
                    {
                        auto& count = std::get<int64_t>(output.back());
                        ++count;
                    }
                    else
                    {
                        output.push_back({1});
                    }
                }
            }
            break;
            case Turn::LEFT:
                output.push_back({Turn::LEFT});
            break;
            case Turn::RIGHT:
                output.push_back({Turn::RIGHT});
            break;
        }
    }

    return output;
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
    
    Point droid{};
    Direction dir{Direction::UP};
    bool falling{false};

    map.push_back({});
    auto* row = &map.front();

    int64_t x{0};
    int64_t y{0};

    while(true)
    {
        auto exec_status = interpreter.Execute();
        if(exec_status == intcode::Interpreter::ExecuteStatusEnum::COMPLETED)
        {
            break;
        }

        char output = static_cast<char>(interpreter.Output());

        switch(output)
        {
            case '#':
                row->emplace_back(Tile{'#', false});
                ++x;

                std::cout << '#';
            break;
            case '.':
                row->emplace_back(Tile{'.', false});
                ++x;

                std::cout << '.';
            break;
            case '\n':
                map.push_back({});
                row = &map.back();
                x = 0;
                ++y;

                std::cout << '\n';
            break;
            case '>':
                row->emplace_back(Tile{'#', false});
                droid.x = x;
                droid.y = y;
                dir = Direction::RIGHT;
                ++x;

                std::cout << '>';
            break;
            case '<':
                row->emplace_back(Tile{'#', false});
                droid.x = x;
                droid.y = y;
                dir = Direction::LEFT;
                ++x;

                std::cout << '<';
            break;
            case '^':
                row->emplace_back(Tile{'#', false});
                droid.x = x;
                droid.y = y;
                dir = Direction::UP;
                ++x;

                std::cout << '^';
            break;
            case 'v':
                row->emplace_back(Tile{'#', false});
                droid.x = x;
                droid.y = y;
                dir = Direction::DOWN;
                ++x;

                std::cout << 'v';
            break;
            case 'X':
                row->emplace_back(Tile{'.', false});
                droid.x = x;
                droid.y = y;
                falling = true;
                ++x;

                std::cout << 'X';
            break;
        }
    }

    find_intersections(map);

    print_map(map, droid, dir, falling);

    Map path = map;

    std::vector<std::tuple<Point, Direction, Turn>> instructions{};
    instructions.emplace_back(Point{droid.x, droid.y}, dir, Turn::STRAIGHT);
    map[droid.y][droid.x].visited = true;

    while(true)
    {
        auto& item = instructions.back();
        Point pos = std::get<Point>(item);;
        Direction dir = std::get<Direction>(item);
        Turn turn = std::get<Turn>(item);

        std::vector<std::tuple<Direction, Turn>> check_dirs{};

        switch(dir)
        {
            case Direction::UP:
            {
                check_dirs.emplace_back(Direction::UP, Turn::STRAIGHT);
                check_dirs.emplace_back(Direction::RIGHT, Turn::RIGHT);
                check_dirs.emplace_back(Direction::LEFT, Turn::LEFT);
            }
            break;
            case Direction::DOWN:
            {
                check_dirs.emplace_back(Direction::DOWN, Turn::STRAIGHT);
                check_dirs.emplace_back(Direction::RIGHT, Turn::LEFT);
                check_dirs.emplace_back(Direction::LEFT, Turn::RIGHT);
            }
            break;
            case Direction::LEFT:
            {
                check_dirs.emplace_back(Direction::LEFT, Turn::STRAIGHT);
                check_dirs.emplace_back(Direction::UP, Turn::RIGHT);
                check_dirs.emplace_back(Direction::DOWN, Turn::LEFT);
            }
            break;
            case Direction::RIGHT:
            {
                check_dirs.emplace_back(Direction::RIGHT, Turn::STRAIGHT);
                check_dirs.emplace_back(Direction::UP, Turn::LEFT);
                check_dirs.emplace_back(Direction::DOWN, Turn::RIGHT);
            }
            break;
        }

        bool found{false};
        for(const auto& check : check_dirs)
        {
            auto c_d = std::get<Direction>(check);
            auto c_t = std::get<Turn>(check);

            try
            {
                switch(c_d)
                {
                    case Direction::UP:
                    {
                        auto& tile = map.at(pos.y - 1).at(pos.x);
                        if((!tile.visited || tile.is_intersection) && tile.c == '#')
                        {
                            tile.visited = true;
                            instructions.emplace_back(Point{pos.x, pos.y - 1}, c_d, c_t);
                            found = true;
                        }
                    }
                    break;
                    case Direction::DOWN:
                    {
                        auto& tile = map.at(pos.y + 1).at(pos.x);
                        if((!tile.visited || tile.is_intersection) && tile.c == '#')
                        {
                            tile.visited = true;
                            instructions.emplace_back(Point{pos.x, pos.y + 1}, c_d, c_t);
                            found = true;
                        }
                    }
                    break;
                    case Direction::LEFT:
                    {
                        auto& tile = map.at(pos.y).at(pos.x - 1);
                        if((!tile.visited || tile.is_intersection) && tile.c == '#')
                        {
                            tile.visited = true;
                            instructions.emplace_back(Point{pos.x - 1, pos.y}, c_d, c_t);
                            found = true;
                        }
                    }
                    break;
                    case Direction::RIGHT:
                    {
                        auto& tile = map.at(pos.y).at(pos.x + 1);
                        if((!tile.visited || tile.is_intersection) && tile.c == '#')
                        {
                            tile.visited = true;
                            instructions.emplace_back(Point{pos.x + 1, pos.y}, c_d, c_t);
                            found = true;
                        }
                    }
                    break;
                }

                // If we found a path, stop looking, each tile should only move
                // once from its current position.
                if(found)
                {
                    break;
                }
            }
            catch(...)
            {
                // .. out of bounds point
            }
        }

        // if there is nowhere to move to, must have reached the end.
        if(!found)
        {
            break;
        }
    }

    std::cout << "Instructions:\n";
    for(const auto& inst : instructions)
    {
        auto p = std::get<Point>(inst);
        auto d = std::get<Direction>(inst);
        auto t = std::get<Turn>(inst);

        std::cout << "[" << p.x << ',' << p.y << "] ";
        switch(d)
        {
            case Direction::UP: std::cout << "UP "; break;
            case Direction::DOWN: std::cout << "DOWN "; break;
            case Direction::LEFT: std::cout << "LEFT "; break;
            case Direction::RIGHT: std::cout << "RIGHT "; break;
        }

        switch(t)
        {
            case Turn::STRAIGHT: std::cout << "S\n"; break; 
            case Turn::LEFT: std::cout << "L\n"; break; 
            case Turn::RIGHT: std::cout << "R\n"; break; 
        }

        // print_map(map, p, d, false);
        // std::this_thread::sleep_for(std::chrono::milliseconds{50});
    }

    auto collapsed = instructions_collapse(instructions);

    std::cout << std::endl;
    std::cout << "Instructions: \n"; 
    for(const auto& v : collapsed)
    {
        if(std::holds_alternative<int64_t>(v))
        {
            auto i = std::get<int64_t>(v);
            std::cout << i << ',';
        }
        else
        {
            auto t = std::get<Turn>(v);
            switch(t)
            {
                case Turn::STRAIGHT: std::cout << "S,"; break;
                case Turn::LEFT: std::cout << "L,"; break;
                case Turn::RIGHT: std::cout << "R,"; break;
            }
        }
    }

    std::cout << std::endl << std::endl;

    // It is known that A and C need to be at the beginning and end
    // of the instruction set, and B will be somewhere in the middle.
    // Manually highlighting and seeing pattersn in an editor proved
    // to find the follow movement routine with A/B/C by hand.
    // It would be interesting to programmatically determine these
    // for a generic solution on any maze.

    std::string main_movement_routine = "A,B,A,C,A,B,A,C,B,C\n";
    std::string a = "R,4,L,12,L,8,R,4\n";
    std::string b = "L,8,R,10,R,10,R,6\n";
    std::string c = "R,4,R,10,L,12\n";

    intcode::Interpreter i2{args[1]};
    i2.Memory(0, 2); // set to function rountine mode

    int64_t input_round{0};

    bool done{false};
    while(!done)
    {
        auto status = i2.Execute();

        switch(status)
        {
            case intcode::Interpreter::ExecuteStatusEnum::COMPLETED:
                done = true;
            break;
            case intcode::Interpreter::ExecuteStatusEnum::OUTPUT_READY:
            {
                char output = static_cast<char>(i2.Output());

                switch(output)
                {
                    case '#':
                        std::cout << '#';
                    break;
                    case '.':
                        std::cout << '.';
                    break;
                    case '\n':
                        std::cout << '\n';
                    break;
                    case '>':
                        std::cout << '>';
                    break;
                    case '<':
                        std::cout << '<';
                    break;
                    case '^':
                        std::cout << '^';
                    break;
                    case 'v':
                        std::cout << 'v';
                    break;
                    case 'X':
                        std::cout << 'X';
                    break;
                    default:
                        if(input_round <= 4)
                        {
                            std::cout << static_cast<char>(i2.Output());
                        }
                        else
                        {
                            // this assumes no video feed, could check
                            // to see what position the droid is in and if
                            // its at the end then switch this this
                            std::cout << i2.Output();
                        }
                    break;
                }
            }
            break;
            case intcode::Interpreter::ExecuteStatusEnum::INPUT_REQUESTED:
            {
                switch(input_round)
                {
                    // main
                    case 0:
                    {
                        bool first{true};
                        for(const auto c : main_movement_routine)
                        {
                            if(first)
                            {
                                first = false;
                            }
                            else
                            {
                                i2.Execute();
                            }
                            std::cout << c;
                            i2.Input(static_cast<int64_t>(c));
                        }
                    }
                    break;
                    // rountines
                    case 1:
                    {
                        bool first{true};
                        for(const auto& c : a)
                        {
                            if(first)
                            {
                                first = false;
                            }
                            else
                            {
                                i2.Execute();
                            }
                            std::cout << c;
                            i2.Input(static_cast<int64_t>(c));
                        }

                    }
                    break;
                    case 2:
                    {
                        bool first{true};
                        for(const auto& c : b)
                        {
                            if(first)
                            {
                                first = false;
                            }
                            else
                            {
                                i2.Execute();
                            }
                            std::cout << c;
                            i2.Input(static_cast<int64_t>(c));
                        }
                    }
                    break;
                    case 3:
                    {
                        bool first{true};
                        for(const auto& c : c)
                        {
                            if(first)
                            {
                                first = false;
                            }
                            else
                            {
                                i2.Execute();
                            }
                            std::cout << c;
                            i2.Input(static_cast<int64_t>(c));
                        }
                    }
                    break;
                    // video feed
                    case 4:
                    {
                        char c = 'n';
                        std::cout << c;
                        i2.Input(static_cast<int64_t>(c));
                        i2.Execute();
                        i2.Input('\n');
                    }
                    break;
                }

                // move to next input type
                input_round++;
            }
            break;
        }
    }

    return 0;
}
