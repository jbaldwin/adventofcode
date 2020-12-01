#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <cmath>

#include <lib/FileUtil.h>
#include <chain/chain.hpp>

enum class Content
{
    SPACE,
    ASTEROID
};

struct Location
{
    Content content{Content::SPACE};
    int64_t x;
    int64_t y;
    double angle{0.0};
    double distance{0.0};

    auto operator<(const Location& l) const -> bool
    {
        if(angle < l.angle) return true;
        if(angle == l.angle)
        {
            if(distance < l.distance)
            {
                return true;
            }
        }
        return false;
    }
};

using Space = 
    std::map<
        int64_t,
        std::map<
            int64_t,
            Location
        >
    >;

auto load_space(
    std::string_view file_name
) -> Space
{
    Space space{};

    auto contents = file::read(file_name);

    int64_t x{0};
    int64_t y{0};
    for(const auto& c : contents)
    {
        if(c == '\n')
        {
            y = 0;
            ++x;
        }
        else
        {
            if(c == '#')
            {
                space[x][y].content = Content::ASTEROID;
            }
            else
            {
                space[x][y].content = Content::SPACE;
            }

            ++y;
        }
    }

    return space;
}

auto print_space_content(
    const Space& space
) -> void
{
    for(const auto& [x, row] : space)
    {
        for(const auto& [y, location] : row)
        {
            switch(location.content)
            {
                case Content::SPACE:
                    std::cout << '.';
                    break;
                case Content::ASTEROID:
                std::cout << '#';
                    break;
            }
        }
        std::cout << "\n";
    }
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    auto space = load_space(args[1]);

    print_space_content(space);

    // Everything revolves around the main asteriod location.
    const int64_t y = 19;
    const int64_t x = 11;

    // Create an ordered set of asteriods based on angle + distance to the main asteroid.
    std::set<Location> locations{};

    for(auto& [y1, row] : space)
    {
        for(auto& [x1, location] : row)
        {
            if(x1 == x && y1 == y || location.content == Content::SPACE)
            {
                continue;
            }

            auto angle = std::atan2(x1 - x, y - y1);

            if(angle < 0)
            {
                angle += 360;
            }

            auto x_diff = x1 - x;
            auto y_diff = y1 - y;

            auto dist = std::sqrt(x_diff * x_diff + y_diff * y_diff);

            location.distance = dist;
            location.angle = angle;
            location.x = x1;
            location.y = y1;

            locations.insert(location);
        }
    }

    std::size_t i = 1;
    while(!locations.empty())
    {
        // The set of 'used angles' to detect if astroids at this angle
        // are blocked on the current pass.
        std::set<double> used_angles{};
        // The set of blocked/not 
        std::set<Location> blocked_locations{};

        for(auto& l : locations)
        {
            // If this angle hasn't been used yet then it gets lasered on this pass.
            if(used_angles.find(l.angle) == used_angles.end())
            {
                used_angles.insert(l.angle);
                std::cout << i << "\t[" << l.x << ',' << l.y << "] " << l.angle << " " << l.distance << std::endl;
                ++i;
            }
            else
            {
                blocked_locations.insert(l);
            }
        }

        // Go through the list in order again and laser the next set of unblocked asteroids.
        locations = blocked_locations;
    }

    return 0;
}
