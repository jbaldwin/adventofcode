#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <set>
#include <cmath>
#include <optional>

using image = std::array<std::array<char, 10>, 10>;
using tile_id = int64_t;

enum class orientation
{
    degrees_0 = 0,
    degrees_90 = 1,
    degrees_180 = 2,
    degrees_270 = 3
};

static const inline std::string orientation_0{"0"};
static const inline std::string orientation_90{"90"};
static const inline std::string orientation_180{"180"};
static const inline std::string orientation_270{"270"};

auto to_string(const orientation& o) -> const std::string&
{
    switch(o)
    {
        case orientation::degrees_0: return orientation_0;
        case orientation::degrees_90: return orientation_90;
        case orientation::degrees_180: return orientation_180;
        case orientation::degrees_270: return orientation_270;
    }
    throw std::runtime_error{"you done goofed"};
}

auto orientation_flip(orientation o) -> orientation
{
    switch(o)
    {
        case orientation::degrees_0: return orientation::degrees_180;
        case orientation::degrees_180: return orientation::degrees_0;
        case orientation::degrees_90: return orientation::degrees_270;
        case orientation::degrees_270: return orientation::degrees_90;
    }
    throw std::runtime_error{"you done goofed again"};
}

struct tile
{
    tile_id id{0};
    orientation o;
    image img{};

    std::vector<std::pair<tile_id, orientation>> left{};
    std::vector<std::pair<tile_id, orientation>> right{};
    std::vector<std::pair<tile_id, orientation>> top{};
    std::vector<std::pair<tile_id, orientation>> bottom{};

    friend auto operator<<(std::ostream& os, const tile& t) -> std::ostream&
    {
        os << "Tile " << t.id << ":\n";
        os << "Orientation: " << to_string(t.o) << ":\n";
        for(const auto& row : t.img)
        {
            for(const auto& col : row)
            {
                os << col;
            }
            os << "\n";
        }

        os << "left [" << chain::str::map_join(
            t.left,
            ", ",
            [](const auto& pair) {
                const auto& [tid, o] = pair;
                return std::to_string(tid) + "-" + to_string(o);
            }
        ) << "]\n";

        os << "right [" << chain::str::map_join(
            t.right,
            ", ",
            [](const auto& pair) {
                const auto& [tid, o] = pair;
                return std::to_string(tid) + "-" + to_string(o);
            }
        ) << "]\n";

        os << "top [" << chain::str::map_join(
            t.top,
            ", ",
            [](const auto& pair) {
                const auto& [tid, o] = pair;
                return std::to_string(tid) + "-" + to_string(o);
            }
        ) << "]\n";

        os << "bottom [" << chain::str::map_join(
            t.bottom,
            ", ",
            [](const auto& pair) {
                const auto& [tid, o] = pair;
                return std::to_string(tid) + "-" + to_string(o);
            }
        ) << "]\n";

        return os;
    }
};

auto tile_by_orientation(
    const std::map<tile_id, std::array<tile, 4>>& tiles,
    const tile_id& tid,
    const orientation& o) -> const tile&
{
    auto tile_set_iter = tiles.find(tid);
    if(tile_set_iter == tiles.end())
    {
        throw std::runtime_error{"invalid tile id " + std::to_string(tid)};
    }

    const auto& tile_set = tile_set_iter->second;

    switch(o)
    {
        case orientation::degrees_0: return tile_set[0];
        case orientation::degrees_90: return tile_set[1];
        case orientation::degrees_180: return tile_set[2];
        case orientation::degrees_270: return tile_set[3];
    }

    throw std::runtime_error{"invalid tile orientation"};
}

auto rotate_tile(const tile& t) -> tile
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

auto parse_input(std::string_view contents) -> std::map<tile_id, std::array<tile, 4>>
{
    std::map<tile_id, std::array<tile, 4>> tiles{};

    auto tile_parts = chain::str::split(contents, "Tile ");

    for(auto& tile_part : tile_parts)
    {
        if(tile_part.empty())
        {
            continue;
        }

        tile t1{};

        auto id_img_parts = chain::str::split(tile_part, ":\n");

        t1.id = chain::str::to_number<tile_id>(id_img_parts[0]).value();
        t1.o = orientation::degrees_0;
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
                t1.img[row_idx][col_idx] = c;
                ++col_idx;
            }
            ++row_idx;
        }

        auto t2 = rotate_tile(t1);
        auto t3 = rotate_tile(t2);
        auto t4 = rotate_tile(t3);

        t2.o = orientation::degrees_90;
        t3.o = orientation::degrees_180;
        t4.o = orientation::degrees_270;

        tiles.emplace(t1.id, std::array<tile, 4>{t1, t2, t3, t4});
    }

    return tiles;
}

auto top_edge_matches(const tile& t1, const tile& t2) -> bool
{
    // Left edge of t1 is compared to right edge of t2.
    size_t t1_x{0};
    size_t t2_x{9};

    bool matched{true};

    for(size_t y = 0; y < 10; ++y)
    {
        if(t1.img[t1_x][y] != t2.img[t2_x][y])
        {
            // std::cout << t1.img[t1_x][y] << " != " << t2.img[t2_x][y] << "\n";
            // std::cout << "done\n";
            matched = false;
            break;
        }
        // std::cout << t1.img[t1_x][y] << " == " << t2.img[t2_x][y] << "\n";
    }
    // std::cout << "done\n";

    if(matched)
    {
        return true;
    }
    matched = true;

    t2_x = 0;

    for(size_t y = 0; y < 10; ++y)
    {
        if(t1.img[t1_x][y] != t2.img[t2_x][y])
        {
            // std::cout << t1.img[t1_x][y] << " != " << t2.img[t2_x][y] << "\n";
            // std::cout << "done\n";
            matched = false;
            break;
        }
        // std::cout << t1.img[t1_x][y] << " == " << t2.img[t2_x][y] << "\n";
    }
    // std::cout << "done\n";

    return matched;
}

auto bottom_edge_matches(const tile& t1, const tile& t2) -> bool
{
    // Right edge of t1 is compared to left edge of t2.
    size_t t1_x{9};
    size_t t2_x{0};

    bool matched{true};

    for(size_t y = 0; y < 10; ++y)
    {
        if(t1.img[t1_x][y] != t2.img[t2_x][y])
        {
            // std::cout << t1.img[t1_x][y] << " != " << t2.img[t2_x][y] << "\n";
            // std::cout << "done\n";
            matched = false;
            break;
        }
        // std::cout << t1.img[t1_x][y] << " == " << t2.img[t2_x][y] << "\n";
    }
    // std::cout << "done\n";

    if(matched)
    {
        return true;
    }
    matched = true;

    t2_x = 9;

    for(size_t y = 0; y < 10; ++y)
    {
        if(t1.img[t1_x][y] != t2.img[t2_x][y])
        {
            // std::cout << t1.img[t1_x][y] << " != " << t2.img[t2_x][y] << "\n";
            // std::cout << "done\n";
            matched = false;
            break;
        }
        // std::cout << t1.img[t1_x][y] << " == " << t2.img[t2_x][y] << "\n";
    }
    // std::cout << "done\n";

    return matched;
}

auto left_edge_matches(const tile& t1, const tile& t2) -> bool
{
    // Top edge of t1 is compared to bottom edge of t2.
    size_t t1_y{0};
    size_t t2_y{9};

    bool matched{true};

    for(size_t x = 0; x < 10; ++x)
    {
        if(t1.img[x][t1_y] != t2.img[x][t2_y])
        {
            // std::cout << t1.img[x][t1_y] << " != " << t2.img[x][t2_y] << "\n";
            // std::cout << "done\n";
            matched = false;
            break;
        }
        // std::cout << t1.img[x][t1_y] << " == " << t2.img[x][t2_y] << "\n";
    }
    // std::cout << "done\n";

    if(matched)
    {
        return true;
    }
    matched = true;

    t2_y = 0;

    for(size_t x = 0; x < 10; ++x)
    {
        if(t1.img[x][t1_y] != t2.img[x][t2_y])
        {
            // std::cout << t1.img[x][t1_y] << " != " << t2.img[x][t2_y] << "\n";
            // std::cout << "done\n";
            matched = false;
            break;
        }
        // std::cout << t1.img[x][t1_y] << " == " << t2.img[x][t2_y] << "\n";
    }
    // std::cout << "done\n";

    return matched;
}

auto right_edge_matches(const tile& t1, const tile& t2) -> bool
{
    // Bottom edge of t1 is compared to top edge of t2.
    size_t t1_y{9};
    size_t t2_y{0};

    bool matched{true};

    for(size_t x = 0; x < 10; ++x)
    {
        if(t1.img[x][t1_y] != t2.img[x][t2_y])
        {
            // std::cout << t1.img[x][t1_y] << " != " << t2.img[x][t2_y] << "\n";
            // std::cout << "done\n";
            matched = false;
            break;
        }
        // std::cout << t1.img[x][t1_y] << " == " << t2.img[x][t2_y] << "\n";
    }
    // std::cout << "done\n";

    if(matched)
    {
        return true;
    }
    matched = true;

    t2_y = 9;

    for(size_t x = 0; x < 10; ++x)
    {
        if(t1.img[x][t1_y] != t2.img[x][t2_y])
        {
            // std::cout << t1.img[x][t1_y] << " != " << t2.img[x][t2_y] << "\n";
            // std::cout << "done\n";
            matched = false;
            break;
        }
        // std::cout << t1.img[x][t1_y] << " == " << t2.img[x][t2_y] << "\n";
    }
    // std::cout << "done\n";

    return matched;
}

auto find_matches(
    std::map<tile_id, std::array<tile, 4>>& tiles
) -> void
{
    for(auto& [tid_current, tile_set_current] : tiles)
    {
        for(auto& tile_current : tile_set_current)
        {
            for(auto& [tid_compare, tile_set_compare] : tiles)
            {
                // Most definitely skip ourself!
                if(tid_current != tid_compare)
                {
                    for(auto& tile_compare : tile_set_compare)
                    {
                        if(left_edge_matches(tile_current, tile_compare))
                        {
                            std::cout << "Found left edge match:\n";
                            std::cout << tile_current << "\n";
                            std::cout << tile_compare << "\n";

                            tile_current.left.emplace_back(tile_compare.id, tile_compare.o);
                        }

                        if(right_edge_matches(tile_current, tile_compare))
                        {
                            std::cout << "Found right edge match:\n";
                            std::cout << tile_current << "\n";
                            std::cout << tile_compare << "\n";

                            tile_current.right.emplace_back(tile_compare.id, tile_compare.o);
                        }

                        if(top_edge_matches(tile_current, tile_compare))
                        {
                            std::cout << "Found top edge match:\n";
                            std::cout << tile_current << "\n";
                            std::cout << tile_compare << "\n";

                            tile_current.top.emplace_back(tile_compare.id, tile_compare.o);
                        }

                        if(bottom_edge_matches(tile_current, tile_compare))
                        {
                            std::cout << "Found bottom edge match:\n";
                            std::cout << tile_current << "\n";
                            std::cout << tile_compare << "\n";

                            tile_current.bottom.emplace_back(tile_compare.id, tile_compare.o);
                        }
                    }
                }
            }
        }
    }
}

using picture = std::vector<std::vector<std::pair<tile_id, orientation>>>;

auto matching_neighbor(
    const std::vector<std::pair<tile_id, orientation>>& neighbor_can_match,
    tile_id tid,
    orientation o
) -> bool
{
    for(const auto& [ntid, no] : neighbor_can_match)
    {
        if(ntid == tid)
        {
            if(no == o)
            {
                return true;
            }

            if(orientation_flip(no) == o)
            {
                return true;
            }

            if(no == orientation_flip(o))
            {
                return true;
            }
        }
    }
    return false;
}

auto slot(
    const std::map<tile_id, std::array<tile, 4>>& tiles,
    std::set<tile_id> remaining_tiles,
    std::set<tile_id> used_tiles,
    picture p,
    size_t x,
    size_t y
) -> void
{
    if(remaining_tiles.empty())
    {
        // found a solution?!
        std::cout << "solution:\n";
        for(const auto& row : p)
        {
            for(const auto& [tid, o] : row)
            {
                std::cout << tid << "\t";
            }
            std::cout << "\n";
        }
        std::cout << "\n";

        size_t max = p.size() - 1;

        auto c1 = p[0][0].first;
        auto c2 = p[0][max].first;
        auto c3 = p[max][0].first;
        auto c4 = p[max][max].first;

        int64_t product = c1 * c2 * c3 * c4;

        std::cout << "The for corners of the apocolypse "
            << c1 << " * " << c2 << " * " << c3 << " * " << c4
            << " = " << product << "\n";
    }
    else
    {
        for(const auto& tid : remaining_tiles)
        {
            if(used_tiles.emplace(tid).second)
            {
                auto tile_set_iter = tiles.find(tid);
                if(tile_set_iter == tiles.end())
                {
                    throw std::runtime_error{"tile id doesn't exist" + std::to_string(tid)};
                }
                const auto& tile_set = tile_set_iter->second;

                for(const auto& tile : tile_set)
                {
                    bool can_slot{true};
                    // Need to check if top and left allow for this tile to be slotted.
                    if(x > 0)
                    {
                        const auto& [x_tile_id, x_tile_o] = p[x - 1][y];

                        if(!matching_neighbor(tile.left, x_tile_id, x_tile_o))
                        {
                            can_slot = false;
                        }
                    }

                    if(can_slot)
                    {
                        if(y > 0)
                        {
                            const auto& [y_tile_id, y_tile_o] = p[x][y - 1];

                            if(!matching_neighbor(tile.top, y_tile_id, y_tile_o))
                            {
                                can_slot = false;
                            }
                        }
                    }

                    if(can_slot)
                    {
                        auto p_copy = p;
                        p_copy[x][y] = std::make_pair(tile.id, tile.o);

                        auto r_copy = remaining_tiles;
                        r_copy.erase(tile.id);

                        auto new_y = y;
                        auto new_x = x;

                        new_y++;
                        if(new_y == p.size())
                        {
                            new_y = 0;
                            new_x++;
                        }

                        slot(tiles, std::move(r_copy), used_tiles, std::move(p_copy), new_x, new_y);
                    }
                }

                used_tiles.erase(tid);
            }
        }
    }
}

auto slot_picture(
    const std::map<tile_id, std::array<tile, 4>>& tiles
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
    for(const auto& [tid, tile_set] : tiles)
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

    find_matches(tiles);

    for(const auto& [id, tile_set] : tiles)
    {
        for(const auto& t : tile_set)
        {
            std::cout << t << "\n";
        }
    }

    slot_picture(tiles);

    return 0;
}
