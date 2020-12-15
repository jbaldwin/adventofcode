#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

#include <lib/file_util.hpp>
#include <chain/chain.hpp>
#include <lib/containers.hpp>

using namespace containers;

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

/**
 * Current direction => Turning towards => New Direction.
 */
static std::map<Direction, std::map<Turn, Direction>> g_turns
{
    {
        Direction::NORTH,
        {
            {Turn::RIGHT, Direction::EAST},
            {Turn::LEFT, Direction::WEST}
        }
    },
    {
        Direction::SOUTH,
        {
            {Turn::RIGHT, Direction::WEST},
            {Turn::LEFT, Direction::EAST}
        }
    },
    {
        Direction::EAST,
        {
            {Turn::RIGHT, Direction::SOUTH},
            {Turn::LEFT, Direction::NORTH}
        }
    },
    {
        Direction::WEST,
        {
            {Turn::RIGHT, Direction::NORTH},
            {Turn::LEFT, Direction::SOUTH}
        }
    },
};

auto walk(
    vec2<int64_t>& pos,
    Direction dir,
    int64_t blocks,
    std::set<vec2<int64_t>>& visited
) -> bool
{
    for(std::size_t i = 0; i < blocks; ++i)
    {
        switch(dir)
        {
            case Direction::NORTH:
                pos.y++;
            break;
            case Direction::SOUTH:
                pos.y--;
            break;
            case Direction::EAST:
                pos.x++;
            break;
            case Direction::WEST:
                pos.x--;
            break;
        }

        auto [iter, inserted] = visited.insert(pos);
        if(!inserted)
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
    }

    auto contents = file::read(args[1]);

    std::vector<Movement> movements{};
    for(const auto& move : chain::str::split(contents, ", "))
    {
        auto turn = (move[0] == 'R') ? Turn::RIGHT : Turn::LEFT;
        auto blocks = std::stoul(std::string{move.substr(1)});

        movements.emplace_back(Movement{turn, blocks});
    }

    std::set<vec2<int64_t>> visited{};

    Direction dir{Direction::NORTH};
    vec2<int64_t> pos{0, 0};
    std::cout << "pos=" << pos << "\n";

    for(const auto& move : movements)
    {
        dir = g_turns[dir][move.turn];

        if(walk(pos, dir, move.blocks, visited))
        {
            break;
        }

        std::cout << move << " pos=" << pos << "\n";
    }

    std::cout << "Visited pos " << pos << " twice.\n";
    std::cout << "Total blocks away = " << (std::abs(pos.x) + std::abs(pos.y)) << "\n";

    return 0;
}
