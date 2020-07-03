#include <iostream>
#include <array>

#include <lib/Containers.hpp>
#include <lib/FileUtil.h>
#include <chain/Chain.hpp>

using Vec2 = containers::Vec2<int64_t>;
using DialPad = std::array<std::array<char, 5>, 5>;

auto in_bounds(
    const DialPad& dial_pad,
    const Vec2& pos,
    const Vec2& dir
    ) -> bool
{
    auto loc = pos + dir;
    if(loc.x >= 0 && loc.x <= 4 && loc.y >= 0 && loc.y <= 4)
    {
        if(dial_pad[loc.x][loc.y] != 'X')
        {
            return true;
        }
    }
    return false;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    static const DialPad dial_pad
    {
        std::array<char, 5>{'X', 'X', '1', 'X', 'X'},
        std::array<char, 5>{'X', '2', '3', '4', 'X'},
        std::array<char, 5>{'5', '6', '7', '8', '9'},
        std::array<char, 5>{'X', 'A', 'B', 'C', 'X'},
        std::array<char, 5>{'X', 'X', 'D', 'X', 'X'}
    };

    std::string code{};

    Vec2 pos{2, 0};

    auto contents = file::read(args[1]);

    auto lines = chain::str::split(contents, '\n');
    code.reserve(lines.size());

    for(const auto& line : lines)
    {
        for(const char c : line)
        {
            switch(c)
            {
                case 'U':
                {
                    std::cout << 'U';
                    Vec2 dir{-1, 0};
                    if(in_bounds(dial_pad, pos, dir))
                    {
                        pos += dir;
                    }
                }
                break;
                case 'D':
                {
                    std::cout << 'D';
                    Vec2 dir{1, 0};
                    if(in_bounds(dial_pad, pos, dir))
                    {
                        pos += dir;
                    }
                }
                break;
                case 'L':
                {
                    std::cout << 'L';
                    Vec2 dir{0, -1};
                    if(in_bounds(dial_pad, pos, dir))
                    {
                        pos += dir;
                    }
                }
                break;
                case 'R':
                {
                    std::cout << 'R';
                    Vec2 dir{0, 1};
                    if(in_bounds(dial_pad, pos, dir))
                    {
                        pos += dir;
                    }
                }
                break;
                default:
                    throw std::runtime_error("Invalid input " + c);
            }
        }
        code += (dial_pad[pos.x][pos.y]);
        std::cout << " [" << code << "]";
        std::cout << '\n';
    }

    return 0;
}
