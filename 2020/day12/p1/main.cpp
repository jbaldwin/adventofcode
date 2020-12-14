#include <lib/FileUtil.h>
#include <lib/containers.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>

enum class direction
{
    north,
    south,
    east,
    west
};

auto to_string(direction d) -> std::string
{
    switch(d)
    {
        case direction::north:
            return "N";
        case direction::south:
            return "S";
        case direction::east:
            return "E";
        case direction::west:
            return "W";
    }
    throw std::runtime_error{"unknown direction"};
}

enum class rotate
{
    left,
    right
};

auto turn(direction d, rotate r, int64_t degrees) -> direction
{
    switch(d)
    {
        case direction::north:
            switch(r)
            {
                case rotate::left:
                    switch(degrees)
                    {
                        case 90:
                            return direction::west;
                        case 180:
                            return direction::south;
                        case 270:
                            return direction::east;
                    }
                    break;
                case rotate::right:
                    switch(degrees)
                    {
                        case 90:
                            return direction::east;
                        case 180:
                            return direction::south;
                        case 270:
                            return direction::west;
                    }
                    break;
            }
            break;
        case direction::south:
            switch(r)
            {
                case rotate::left:
                    switch(degrees)
                    {
                        case 90:
                            return direction::east;
                        case 180:
                            return direction::north;
                        case 270:
                            return direction::west;
                    }
                    break;
                case rotate::right:
                    switch(degrees)
                    {
                        case 90:
                            return direction::west;
                        case 180:
                            return direction::north;
                        case 270:
                            return direction::east;
                    }
                    break;
            }
            break;
        case direction::east:
            switch(r)
            {
                case rotate::left:
                    switch(degrees)
                    {
                        case 90:
                            return direction::north;
                        case 180:
                            return direction::west;
                        case 270:
                            return direction::south;
                    }
                    break;
                case rotate::right:
                    switch(degrees)
                    {
                        case 90:
                            return direction::south;
                        case 180:
                            return direction::west;
                        case 270:
                            return direction::north;
                    }
                    break;
            }
            break;
        case direction::west:
            switch(r)
            {
                case rotate::left:
                    switch(degrees)
                    {
                        case 90:
                            return direction::south;
                        case 180:
                            return direction::east;
                        case 270:
                            return direction::north;
                    }
                    break;
                case rotate::right:
                    switch(degrees)
                    {
                        case 90:
                            return direction::north;
                        case 180:
                            return direction::east;
                        case 270:
                            return direction::south;
                    }
                    break;
            }
            break;
    }
    throw std::logic_error{"shouldn't ever get here"};
}

using vec2 = containers::vec2<int64_t>;

struct ship
{
    vec2 pos{0, 0};
    direction dir{direction::east};

    friend auto operator<<(std::ostream& os, const ship& s) -> std::ostream&
    {
        os << s.pos << " dir=[ " << to_string(s.dir) << "]";
        return os;
    }
};

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);
    auto lines = chain::str::split(contents, '\n');

    ship s{};

    for(auto& line : lines)
    {
        char first = line[0];
        line.remove_prefix(1);
        int64_t v = chain::str::to_number<int64_t>(line).value();

        switch(first)
        {
            case 'N':
                s.pos.y += v;
                break;
            case 'S':
                s.pos.y -= v;
                break;
            case 'E':
                s.pos.x += v;
                break;
            case 'W':
                s.pos.x -= v;
                break;
            case 'L':
                s.dir = turn(s.dir, rotate::left, v);
                break;
            case 'R':
                s.dir = turn(s.dir, rotate::right, v);
                break;
            case 'F':
                switch(s.dir)
                {
                    case direction::north:
                        s.pos.y += v;
                        break;
                    case direction::south:
                        s.pos.y -= v;
                        break;
                    case direction::east:
                        s.pos.x += v;
                        break;
                    case direction::west:
                        s.pos.x -= v;
                        break;
                }
                break;
            default:
                throw std::runtime_error{"unknown action"};
        }

        std::cout << s << "\n";
    }

    std::cout << s << "\n";
    std::cout << "manhattan distance = " << (std::abs(s.pos.x) + std::abs(s.pos.y)) << "\n";

    return 0;
}
