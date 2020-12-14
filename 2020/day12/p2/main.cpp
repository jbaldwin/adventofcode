#include <lib/FileUtil.h>
#include <lib/containers.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>

using vec2 = containers::vec2<int64_t>;

enum class rotate
{
    left,
    right
};

auto rotate_point(const vec2& ship, vec2& waypoint, rotate r, int64_t degrees) -> void
{
    switch(r)
    {
        case rotate::left:
            switch(degrees)
            {
                case 90:
                {
                    auto x = waypoint.x;
                    waypoint.x = -waypoint.y;
                    waypoint.y = x;
                }
                    break;
                case 180:
                {
                    waypoint.x = -waypoint.x;
                    waypoint.y = -waypoint.y;
                }
                    break;
                case 270:
                {
                    auto x = waypoint.x;
                    waypoint.x = waypoint.y;
                    waypoint.y = -x;
                }
                    break;
            }
            break;
        case rotate::right:
            switch(degrees)
            {
                case 90:
                {
                    auto x = waypoint.x;
                    waypoint.x = waypoint.y;
                    waypoint.y = -x;
                }
                    break;
                case 180:
                {
                    waypoint.x = -waypoint.x;
                    waypoint.y = -waypoint.y;
                }
                    break;
                case 270:
                {
                    auto x = waypoint.x;
                    waypoint.x = -waypoint.y;
                    waypoint.y = x;
                }
                    break;
            }
            break;
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
    auto lines = chain::str::split(contents, '\n');

    vec2 ship{0, 0};
    vec2 waypoint{10, 1};
    std::cout << ship << " " << waypoint << "\n";

    for(auto& line : lines)
    {
        char first = line[0];
        line.remove_prefix(1);
        int64_t v = chain::str::to_number<int64_t>(line).value();

        switch(first)
        {
            case 'N':
                waypoint.y += v;
                break;
            case 'S':
                waypoint.y -= v;
                break;
            case 'E':
                waypoint.x += v;
                break;
            case 'W':
                waypoint.x -= v;
                break;
            case 'L':
                rotate_point(ship, waypoint, rotate::left, v);
                break;
            case 'R':
                rotate_point(ship, waypoint, rotate::right, v);
                break;
            case 'F':
                ship.x += (waypoint.x * v);
                ship.y += (waypoint.y * v);
                break;
            default:
                throw std::runtime_error{"unknown action"};
        }

        std::cout << ship << " " << waypoint << "\n";
    }

    std::cout << ship << "\n";
    std::cout << "manhattan distance = " << (std::abs(ship.x) + std::abs(ship.y)) << "\n";

    return 0;
}
