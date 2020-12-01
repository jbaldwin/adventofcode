#include <iostream>
#include <vector>
#include <string>
#include <map>

#include <lib/FileUtil.h>
#include <chain/chain.hpp>
#include <lib/Algorithms.hpp>

enum class Content
{
    SPACE,
    ASTEROID
};

struct Location
{
    Content content{Content::SPACE};
    int64_t visible{0};
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
    Space& space
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

auto print_space_visible(
    Space& space
) -> void
{
    for(const auto& [x, row] : space)
    {
        for(const auto& [y, location] : row)
        {
            std::cout << location.visible;
        }
        std::cout << "\n";
    }
}

auto is_blocked(
    Space& space,
    int64_t x,
    int64_t y,
    int64_t x1,
    int64_t y1,
    int64_t n,
    int64_t d
) -> bool
{
    while(true)
    {
        x1 += n;
        y1 += d;

        // Direct line of sight!
        if(x1 == x && y1 == y)
        {
            return false;
        }

        // We've run into an astroid :(
        if(space[x1][y1].content == Content::ASTEROID)
        {
            return true;
        }
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

    for(auto& [x, row] : space)
    {
        for(auto& [y, location] : row)
        {
            // Skip all locations that are not astroids.
            if(location.content == Content::SPACE)
            {
                continue;
            }

            for(const auto& [x1, row1] : space)
            {
                for(const auto& [y1, location1] : row1)
                {
                    // Skip ourself and any locations that are not astroids.
                    if(x == x1 && y == y1 || location1.content == Content::SPACE)
                    {
                        continue;
                    }

                    // Calculate the manhattan distance between the two asteroids
                    // and then reduce the fraction to get the x and y step size.
                    // Each minimum step is checked to see if an astroid is in the
                    // way blocking visibility between the two original asteroids.
                    // Note that the fraction_reduce function uses the gcd and
                    // mangles the original sign on the x,y steps, so preverse
                    // the original sign and re-apply to the reduced fraction.
                    int64_t n = (x - x1);
                    int64_t d = (y - y1);

                    bool n_neg = (n < 0);
                    bool d_neg = (d < 0);

                    algo::fraction_reduce(n, d);

                    if(n_neg && n > 0) 
                    {
                        n *= -1;
                    }
                    if(!n_neg && n < 0)
                    {
                        n *= -1;
                    }

                    if(d_neg && d > 0)
                    {
                        d *= -1;
                    }
                    if(!d_neg && d < 0)
                    {
                        d *= -1;
                    }

                    if(!is_blocked(space, x, y, x1, y1, n, d))
                    {
                        location.visible++;
                    }
                }
            }
        }
    }

    int64_t max_visible{0};
    int64_t max_x{0};
    int64_t max_y{0};

    for(const auto& [x, row] : space)
    {
        for(const auto& [y, location] : row)
        {
            if(location.visible > max_visible)
            {
                max_visible = location.visible;
                max_x = x;
                max_y = y;
            }
        }
    }

    std::cout << '[' << max_x << ',' << max_y << "] " << max_visible << std::endl;

    return 0;
}