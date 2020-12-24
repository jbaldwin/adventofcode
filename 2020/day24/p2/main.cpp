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

using hexgrid = std::map<int64_t, std::map<int64_t, std::map<int64_t, tile_t>>>;

static std::array<direction_t, 6> directions{
    direction_t::east,
    direction_t::southeast,
    direction_t::southwest,
    direction_t::west,
    direction_t::northwest,
    direction_t::northeast
};

auto next_day(const hexgrid& input, bool flip_tiles = true) -> hexgrid
{
    auto output = input;

    for(const auto& [x_idx, x_tiles] : input)
    {
        for(const auto& [y_idx, y_tiles] : x_tiles)
        {
            for(const auto& [z_idx, i_tile] : y_tiles)
            {
                uint64_t bcount{0};

                vec3 pos{x_idx, y_idx, z_idx};
                for(const auto& d : directions)
                {
                    auto n_pos = pos + cube_directions(d);

                    const tile_t* i_n_tile{nullptr};

                    auto x_iter = input.find(n_pos.x);
                    if(x_iter != input.end())
                    {
                        auto y_iter = x_iter->second.find(n_pos.y);
                        if(y_iter != x_iter->second.end())
                        {
                            auto z_iter = y_iter->second.find(n_pos.z);
                            if(z_iter != y_iter->second.end())
                            {
                                i_n_tile = &z_iter->second;
                            }
                        }
                    }

                    if(i_n_tile != nullptr)
                    {
                        if(i_n_tile->color == color_t::black)
                        {
                            bcount++;
                        }
                    }

                    // Grow outwards on the output hexgrid.
                    output[n_pos.x][n_pos.y][n_pos.z];
                }

                if(flip_tiles)
                {
                    auto& o_tile = output[x_idx][y_idx][z_idx];
                    if(i_tile.color == color_t::white)
                    {
                        // White tiles with exactly 2 black tiles flips to black.
                        if(bcount == 2)
                        {
                            o_tile.color = color_t::black;
                        }
                    }
                    else // color_t::black
                    {
                        // Black tiles with 'zero' OR more than '2' black tiles flips to white
                        if(bcount == 0 || bcount > 2)
                        {
                            o_tile.color = color_t::white;
                        }
                    }
                }
            }
        }
    }

    return output;
}

auto grid_count_black_tiles(const hexgrid& input) -> size_t
{
    size_t bcount{0};
    for(const auto& [x_idx, x_tiles] : input)
    {
        for(const auto& [y_idx, y_tiles] : x_tiles)
        {
            for(const auto& [z_idx, tile] : y_tiles)
            {
                if(tile.color == color_t::black)
                {
                    ++bcount;
                }
            }
        }
    }
    return bcount;
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

        tile.color = (tile.color == color_t::white) ? color_t::black : color_t::white;
    }

    std::cout << "\nDay 0:" << grid_count_black_tiles(tiles) << "\n";

    // The grid needs to be expanded so the first day can properly count, after that its always
    // "big enough" around all the tiles to properly count.  This call will just make the hexgrid
    // larger but it won't flip or change any tiles yet.
    tiles = next_day(tiles, false);

    for(size_t i = 1; i <= 100; ++i)
    {
        tiles = next_day(tiles);
        std::cout << "Day " << i << ": " << grid_count_black_tiles(tiles) << "\n";
    }

    return 0;
}
