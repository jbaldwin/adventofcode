#include <lib/FileUtil.h>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>

enum class pos : char
{
    floor = '.',
    empty = 'L',
    occupied = '#'
};

using seats = std::vector<std::vector<pos>>;

auto operator<<(std::ostream& os, const seats& seats) -> std::ostream&
{
    for(size_t x = 0; x < seats.size(); ++x)
    {
        for(size_t y = 0; y < seats[0].size(); ++y)
        {
            os << static_cast<char>(seats[x][y]);
        }
        os << "\n";
    }
    // for(const auto& row : seats)
    // {
    //     for(const auto& chair : row)
    //     {
    //         os << static_cast<char>(chair);
    //     }
    //     os << "\n";
    // }
    return os;
}

auto seats_occupied(const seats& seats) -> size_t
{
    size_t occupied{0};
    for(const auto& row : seats)
    {
        for(const auto& chair : row)
        {
            if(chair == pos::occupied)
            {
                ++occupied;
            }
        }
    }
    return occupied;
}

static std::array<std::pair<int64_t, int64_t>, 8> neighbors = {
    std::pair{-1, -1},   std::pair{0, -1},    std::pair{1, -1},
    std::pair{-1, 0},                         std::pair{1, 0},
    std::pair{-1, 1},    std::pair{0, 1},     std::pair{1, 1}
};

static auto step(const seats& s) -> seats
{
    seats next = s;

    auto x_max = s.size();
    auto y_max = s[0].size();

    for(size_t x = 0; x < x_max; ++x)
    {
        for(size_t y = 0; y < y_max; ++y)
        {
            auto type = s[x][y];
            if(type == pos::floor)
            {
                continue;
            }

            size_t occupied{0};

            for(const auto& [n_x, n_y] : neighbors)
            {
                auto x1 = x + n_x;
                auto y1 = y + n_y;

                if(x1 >= 0 && x1 < x_max && y1 >= 0 && y1 < y_max)
                {
                    if(s[x1][y1] == pos::occupied)
                    {
                        ++occupied;
                    }
                }
            }

            switch(type)
            {
                case pos::empty:
                    if(occupied == 0)
                    {
                        next[x][y] = pos::occupied;
                    }
                    break;
                case pos::occupied:
                    if(occupied >= 4)
                    {
                        next[x][y] = pos::empty;
                    }
                    break;
            }
        }
    }

    return next;
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

    // If a seat is empty (L), and there are NO occupied seats adjacentto it,
    //   then the sea becomes occupied.
    // If a seat is occupied (#) and four or more seats adjacent to it are
    //   also occupied, the seat becomes empty.
    // Otherwise, the seat's state does not change.

    seats current{};
    for(const auto& line : lines)
    {
        auto& row = current.emplace_back();
        for(const auto& c : line)
        {
            switch(c)
            {
                case '.':
                    row.push_back(pos::floor);
                    break;
                case 'L':
                    row.push_back(pos::empty);
                    break;
                case '#':
                    row.push_back(pos::occupied);
                    break;
            }
        }
    }

    seats prev{};
    do
    {
        std::cout << current << "\n\n";
        prev = current;
        current = step(current);
    } while(prev != current);

    std::cout << "occupied seats = " << seats_occupied(current) << "\n";

    return 0;
}
