#include <iostream>
#include <array>

#include <lib/Containers.hpp>
#include <lib/FileUtil.h>
#include <chain/chain.hpp>

using Vec2 = containers::Vec2<int64_t>;

auto in_bounds(
    const Vec2& pos,
    const Vec2& dir
    ) -> bool
{
    auto loc = pos + dir;
    return (loc.x >= 0 && loc.x <= 2 && loc.y >= 0 && loc.y <= 2);
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    static const std::array<std::array<char, 3>, 3> dial_pad
    {
        std::array<char, 3>{'1', '2', '3'},
        std::array<char, 3>{'4', '5', '6'},
        std::array<char, 3>{'7', '8', '9'}
    };

    std::string code{};

    Vec2 pos{1, 1};

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
                    if(in_bounds(pos, dir))
                    {
                        pos += dir;
                    }
                }
                break;
                case 'D':
                {
                    std::cout << 'D';
                    Vec2 dir{1, 0};
                    if(in_bounds(pos, dir))
                    {
                        pos += dir;
                    }
                }
                break;
                case 'L':
                {
                    std::cout << 'L';
                    Vec2 dir{0, -1};
                    if(in_bounds(pos, dir))
                    {
                        pos += dir;
                    }
                }
                break;
                case 'R':
                {
                    std::cout << 'R';
                    Vec2 dir{0, 1};
                    if(in_bounds(pos, dir))
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
