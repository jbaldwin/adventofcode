#include <lib/file_util.hpp>
#include <chain/chain.hpp>
#include <lib/containers.hpp>

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <memory>
#include <map>

using vec3 = containers::vec3<int64_t>;

enum class color_t
{
    white,
    black
};

auto operator<<(std::ostream& os, const color_t& c) -> std::ostream&
{
    if(c == color_t::white)
    {
        os << "white";
    }
    else
    {
        os << "black";
    }
    return os;
}

enum class direction_t : size_t
{
    east = 0,
    southeast = 1,
    southwest = 2,
    west = 3,
    northwest = 4,
    northeast = 5
};

auto to_string(const direction_t& d) -> std::string
{
    switch(d)
    {
        case direction_t::east: return "e";
        case direction_t::southeast: return "se";
        case direction_t::southwest: return "sw";
        case direction_t::west: return "w";
        case direction_t::northwest: return "nw";
        case direction_t::northeast: return "ne";
    }
    throw std::runtime_error{"invalid direction"};
}

auto operator<<(std::ostream& os, const direction_t d) -> std::ostream&
{
    os << to_string(d);
    return os;
}

auto cube_directions(const direction_t& d) -> vec3
{
    switch(d)
    {
        case direction_t::east:         return vec3{1, -1, 0};
        case direction_t::southeast:    return vec3{0, -1, 1};
        case direction_t::southwest:    return vec3{-1, 0, 1};
        case direction_t::west:         return vec3{-1, 1, 0};
        case direction_t::northwest:    return vec3{0, 1, -1};
        case direction_t::northeast:    return vec3{1, 0, -1};
    }
    throw std::runtime_error{"invalid direction"};
}

struct tile_t
{
    color_t color{color_t::white};
    bool finishing_tile{false};
};

auto parse_next(std::string_view& data) -> direction_t
{
    if(data[0] == 'e')
    {
        data.remove_prefix(1);
        return direction_t::east;
    }
    else if(data[0] == 'w')
    {
        data.remove_prefix(1);
        return direction_t::west;
    }
    else
    {
        if(data[0] == 's')
        {
            if(data[1] == 'e')
            {
                data.remove_prefix(2);
                return direction_t::southeast;
            }
            else if(data[1] == 'w')
            {
                data.remove_prefix(2);
                return direction_t::southwest;
            }
            else
            {
                throw std::runtime_error{std::string{"invalid next direction from 's' ="} + std::string{data}};
            }
        }
        else if(data[0] == 'n')
        {
            if(data[1] == 'e')
            {
                data.remove_prefix(2);
                return direction_t::northeast;
            }
            else if(data[1] == 'w')
            {
                data.remove_prefix(2);
                return direction_t::northwest;
            }
            else
            {
                throw std::runtime_error{std::string{"invalid next direction from 'n' ="} + std::string{data}};
            }
        }
    }

    throw std::runtime_error{std::string{"invalid next direction = "} + std::string{data}};
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

    std::map<int64_t, std::map<int64_t, std::map<int64_t, tile_t>>> tiles{};

    for(auto& directions : chain::str::split(contents, '\n'))
    {
        // Every set of directions starts at the center tile.
        vec3 pos{0, 0, 0};

        while(!directions.empty())
        {
            auto d = parse_next(directions);
            std::cout << d;

            pos += cube_directions(d);
        }

        auto& tile = tiles[pos.x][pos.y][pos.z];

        std::cout << " " << pos;
        std::cout << " was " << tile.color;

        tile.finishing_tile = true;
        tile.color = (tile.color == color_t::white) ? color_t::black : color_t::white;

        std::cout << " is now " << tile.color << "\n";
    }

    size_t finishing_tiles{0};
    size_t wcount{0};
    size_t bcount{0};

    for(const auto& [x_idx, x_tiles] : tiles)
    {
        for(const auto& [y_idx, y_tiles] : x_tiles)
        {
            for(const auto& [z_idx, tile] : y_tiles)
            {
                vec3 pos{x_idx, y_idx, z_idx};
                std::cout << pos << " " << tile.color << " " << std::boolalpha << tile.finishing_tile << "\n";
                if(tile.finishing_tile)
                {
                    ++finishing_tiles;
                    if(tile.color == color_t::white)
                    {
                        wcount++;
                    }
                    else
                    {
                        bcount++;
                    }
                }
            }
        }
    }

    std::cout << "Finishing tiles = " << finishing_tiles << "\n";
    std::cout << "White tiles = " << wcount << "\n";
    std::cout << "Black tiles = " << bcount << "\n";

    return 0;
}
