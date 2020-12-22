#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <set>
#include <cmath>

using image = std::array<std::array<char, 10>, 10>;
using tile_id = int64_t;

struct tile
{
    tile_id id{0};
    image img{};

    friend auto operator<<(std::ostream& os, const tile& t) -> std::ostream&
    {
        os << "Tile " << t.id << ":\n";
        for(const auto& row : t.img)
        {
            for(const auto& col : row)
            {
                os << col;
            }
            os << "\n";
        }

        return os;
    }
};

auto tile_rotate_90(const tile& t) -> tile
{
    tile t1{};
    t1.id = t.id;

    size_t rotate_y{9};
    for(size_t x = 0; x < 10; ++x)
    {
        for(size_t y = 0; y < 10; ++y)
        {
            t1.img[y][rotate_y] = t.img[x][y];
        }

        --rotate_y;
    }
    return t1;
}


// X -> Y
auto tile_flip_horizontal(const tile& t) -> tile
{
    tile t1{};
    t1.id = t.id;

    for(size_t x = 0; x < 10; ++x)
    {
        for(size_t y = 0; y < 10; ++y)
        {
            t1.img[x][9 - y] = t.img[x][y];
        }
    }

    return t1;
}

// X
// |
// V
// Y
auto tile_flip_vertical(const tile& t) -> tile
{
    tile t1{};
    t1.id = t.id;

    for(size_t x = 0; x < 10; ++x)
    {
        for(size_t y = 0; y < 10; ++y)
        {
            t1.img[9 - x][y] = t.img[x][y];
        }
    }

    return t1;
}

auto parse_input(std::string_view contents) -> std::map<tile_id, tile>
{
    std::map<tile_id, tile> tiles{};

    auto tile_parts = chain::str::split(contents, "Tile ");

    for(auto& tile_part : tile_parts)
    {
        if(tile_part.empty())
        {
            continue;
        }

        tile t{};

        auto id_img_parts = chain::str::split(tile_part, ":\n");

        t.id = chain::str::to_number<tile_id>(id_img_parts[0]).value();
        auto img_part = id_img_parts[1];

        size_t row_idx{0};
        for(const auto& row : chain::str::split(img_part, '\n'))
        {
            if(row.empty())
            {
                continue;
            }

            size_t col_idx{0};
            for(const auto& c : row)
            {
                t.img[row_idx][col_idx] = c;
                ++col_idx;
            }
            ++row_idx;
        }

        tiles.emplace(t.id, t);
    }

    return tiles;
}

auto top_edge_matches(const tile& t1, const tile& t2) -> bool
{
    // Top edge of t1 is compared to bottom edge of t2.
    size_t t1_x{0};
    size_t t2_x{9};

    for(size_t y = 0; y < 10; ++y)
    {
        if(t1.img[t1_x][y] != t2.img[t2_x][y])
        {
            return false;
        }
    }
    return true;
}

auto left_edge_matches(const tile& t1, const tile& t2) -> bool
{
    // Left edge of t1 is compared to right edge of t2.
    size_t t1_y{0};
    size_t t2_y{9};

    for(size_t x = 0; x < 10; ++x)
    {
        if(t1.img[x][t1_y] != t2.img[x][t2_y])
        {
            return false;
        }
    }
    return true;
}

using picture = std::vector<std::vector<tile>>;

auto slot(
    const std::map<tile_id, tile>& tiles,
    std::set<tile_id> remaining_tiles,
    std::set<tile_id> used_tiles,
    picture p,
    size_t x,
    size_t y
) -> void;

auto can_slot(
    const picture& p,
    const tile& t,
    size_t x,
    size_t y
) -> bool
{
    if(x > 0)
    {
        const auto& left_tile = p[x - 1][y];

        if(!left_edge_matches(t, left_tile))
        {
            return false;
        }
    }

    if(y > 0)
    {
        const auto& above_tile = p[x][y - 1];

        if(!top_edge_matches(t, above_tile))
        {
            return false;
        }
    }

    return true;
}

static std::stringstream g_visited_builder{};
static std::set<std::string> g_visited{};
static bool g_found{false};

auto do_slot(
    const std::map<tile_id, tile>& tiles,
    std::set<tile_id> remaining_tiles,
    std::set<tile_id> used_tiles,
    picture p,
    size_t x,
    size_t y,
    const tile& t
) -> void
{
    p[x][y] = t;

    remaining_tiles.erase(t.id);

    auto new_y = y;
    auto new_x = x;

    new_y++;
    if(new_y == p.size())
    {
        new_y = 0;
        new_x++;
    }

    g_visited_builder.clear();
    g_visited_builder.str("");
    for(const auto& row : p)
    {
        for(const auto& t : row)
        {
            g_visited_builder << t.id << "-";
        }
    }

    // This is key, otherwise there are far too many duplicates to process in any reasonable amount of time!
    if(g_visited.emplace(g_visited_builder.str()).second)
    {
        slot(tiles, std::move(remaining_tiles), std::move(used_tiles), std::move(p), new_x, new_y);
    }

}

auto slot(
    const std::map<tile_id, tile>& tiles,
    std::set<tile_id> remaining_tiles,
    std::set<tile_id> used_tiles,
    picture p,
    size_t x,
    size_t y
) -> void
{
    if(g_found)
    {
        return;
    }

    if(remaining_tiles.empty())
    {
        // found a solution?!
        std::cout << "solution:\n";
        for(const auto& row : p)
        {
            for(const auto& t : row)
            {
                std::cout << t.id << "\t";
            }
            std::cout << "\n";
        }
        std::cout << "\n";

        size_t max = p.size() - 1;

        auto c1 = p[0][0].id;
        auto c2 = p[0][max].id;
        auto c3 = p[max][0].id;
        auto c4 = p[max][max].id;

        int64_t product = c1 * c2 * c3 * c4;

        std::cout << "The for corners of the apocolypse "
            << c1 << " * " << c2 << " * " << c3 << " * " << c4
            << " = " << product << "\n";

        g_found = true;
    }
    else
    {
        for(const auto& tid : remaining_tiles)
        {
            if(used_tiles.emplace(tid).second)
            {
                auto t = tiles.find(tid)->second;

                // Rotation 0
                if(can_slot(p, t, x, y))
                {
                    do_slot(tiles, remaining_tiles, used_tiles, p, x, y, t);
                }

                // Flip horizontal
                auto t_h = tile_flip_horizontal(t);
                if(can_slot(p, t_h, x, y))
                {
                    do_slot(tiles, remaining_tiles, used_tiles, p, x, y, t_h);
                }

                auto t_v = tile_flip_vertical(t);
                if(can_slot(p, t_v, x, y))
                {
                    do_slot(tiles, remaining_tiles, used_tiles, p, x, y, t_v);
                }

                // repeat for each rotation

                // Rotation 90
                t = tile_rotate_90(t);
                if(can_slot(p, t, x, y))
                {
                    do_slot(tiles, remaining_tiles, used_tiles, p, x, y, t);
                }

                // Flip horizontal
                t_h = tile_flip_horizontal(t);
                if(can_slot(p, t_h, x, y))
                {
                    do_slot(tiles, remaining_tiles, used_tiles, p, x, y, t_h);
                }

                 t_v = tile_flip_vertical(t);
                if(can_slot(p, t_v, x, y))
                {
                    do_slot(tiles, remaining_tiles, used_tiles, p, x, y, t_v);
                }

                // Rotation 180
                t = tile_rotate_90(t);
                if(can_slot(p, t, x, y))
                {
                    do_slot(tiles, remaining_tiles, used_tiles, p, x, y, t);
                }

                // Flip horizontal
                t_h = tile_flip_horizontal(t);
                if(can_slot(p, t_h, x, y))
                {
                    do_slot(tiles, remaining_tiles, used_tiles, p, x, y, t_h);
                }

                t_v = tile_flip_vertical(t);
                if(can_slot(p, t_v, x, y))
                {
                    do_slot(tiles, remaining_tiles, used_tiles, p, x, y, t_v);
                }

                // Rotation 270
                t = tile_rotate_90(t);
                if(can_slot(p, t, x, y))
                {
                    do_slot(tiles, remaining_tiles, used_tiles, p, x, y, t);
                }

                // Flip horizontal
                t_h = tile_flip_horizontal(t);
                if(can_slot(p, t_h, x, y))
                {
                    do_slot(tiles, remaining_tiles, used_tiles, p, x, y, t_h);
                }

                t_v = tile_flip_vertical(t);
                if(can_slot(p, t_v, x, y))
                {
                    do_slot(tiles, remaining_tiles, used_tiles, p, x, y, t_v);
                }


                used_tiles.erase(tid);
            }
        }
    }
}

auto slot_picture(
    const std::map<tile_id, tile>& tiles
) -> void
{
    size_t side_length = std::sqrt(tiles.size());

    picture p{};
    p.resize(side_length);
    for(auto& row : p)
    {
        row.resize(side_length);
    }

    std::set<tile_id> remaining_tiles{};
    for(const auto& [tid, tile] : tiles)
    {
        remaining_tiles.emplace(tid);
    }

    slot(tiles, std::move(remaining_tiles), {}, p, 0, 0);
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

    auto tiles = parse_input(contents);

    slot_picture(tiles);

    return 0;
}
