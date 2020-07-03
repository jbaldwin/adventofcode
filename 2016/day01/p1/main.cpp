#include <iostream>
#include <vector>
#include <string>
#include <map>

#include <lib/FileUtil.h>
#include <chain/Chain.hpp>
#include <lib/Containers.hpp>

enum class Direction
{
    NORTH,
    SOUTH,
    EAST,
    WEST
};

enum class Turn
{
    RIGHT,
    LEFT
};

struct Movement
{
    Turn turn;
    uint64_t blocks;

    friend auto operator<<(std::ostream& os, const Movement& m) -> std::ostream&
    {
        if(m.turn == Turn::RIGHT)
        {
            os << "R";
        }
        else
        {
            os << "L";
        }
        os << m.blocks;
        return os;
    }
};

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
    }

    auto contents = file::read(args[1]);

    std::vector<Movement> movements{};
    for(const auto& move : chain::str::split(contents, ", "))
    {
        auto turn = (move[0] == 'R') ? Turn::RIGHT : Turn::LEFT;
        auto blocks = std::stoul(std::string{move.substr(1)});

        movements.emplace_back(Movement{turn, blocks});
    }

    using namespace containers;

    Direction dir{Direction::NORTH};
    Vec2<int64_t> pos{0, 0};
    std::cout << "pos=" << pos << "\n";

    for(const auto& move : movements)
    {
        switch(dir)
        {
            case Direction::NORTH:
            {
                if(move.turn == Turn::RIGHT)
                {
                    dir = Direction::EAST;
                    pos.x += move.blocks;
                }
                else
                {
                    dir = Direction::WEST;
                    pos.x -= move.blocks;
                }
            }
            break;
            case Direction::SOUTH:
            {
                if(move.turn == Turn::RIGHT)
                {
                    dir = Direction::WEST;
                    pos.x -= move.blocks;
                }
                else
                {
                    dir = Direction::EAST;
                    pos.x += move.blocks;
                }
            }
            break;
            case Direction::EAST:
            {
                if(move.turn == Turn::RIGHT)
                {
                    dir = Direction::SOUTH;
                    pos.y -= move.blocks;
                }
                else
                {
                    dir = Direction::NORTH;
                    pos.y += move.blocks;
                }
            }
            break;
            case Direction::WEST:
            {
                if(move.turn == Turn::RIGHT)
                {
                    dir = Direction::NORTH;
                    pos.y += move.blocks;
                }
                else
                {
                    dir = Direction::SOUTH;
                    pos.y -= move.blocks;
                }
            }
            break;
        }

        std::cout << move << " pos=" << pos << "\n";
    }

    std::cout << "Final pos=" << pos << "\n";
    std::cout << "Total blocks away = " << (std::abs(pos.x) + std::abs(pos.y)) << "\n";

    return 0;
}
