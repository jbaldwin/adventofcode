#include <lib/file_util.hpp>
#include <lib/containers.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <map>

using coordinate = containers::vec3<int64_t>;

enum class state : uint8_t
{
    inactive = 0,
    active = 1
};

auto to_string(const state& s) -> char
{
    return (s == state::active) ? '#' : '.';
}

using zplane = std::map<int64_t, state>;
using yplane = std::map<int64_t, zplane>;
using xplane = std::map<int64_t, yplane>;
using space = xplane;

auto operator<<(std::ostream& os, const space& s) -> std::ostream&
{
    for(const auto& [z, zdata] : s)
    {
        os << "z=" << z << "\n";
        for(const auto& [x, xdata] : zdata)
        {
            for(const auto& [y, state] : xdata)
            {
                os << to_string(state);
            }
            os << "\n";
        }
    }

    return os;
}

auto generate_neighbors(coordinate c) -> std::vector<coordinate>
{
    std::vector<coordinate> neighbors{};

    for(int64_t z = -1; z <= 1; ++z)
    {
        for(int64_t x = -1; x <= 1; ++ x)
        {
            for(int64_t y = -1; y <= 1; ++y)
            {
                coordinate c_prime{c.x + x, c. y + y, c.z + z};

                if(c == c_prime)
                {
                    continue;
                }
                else
                {
                    neighbors.emplace_back(c_prime);
                }
            }
        }
    }

    return neighbors;
}

auto expand(const space& s) -> space
{
    auto s1 = s;

    for(const auto& [z, zdata] : s)
    {
        for(const auto& [x, xdata] : zdata)
        {
            for(const auto& [y, state] : xdata)
            {
                for(const auto& c : generate_neighbors(coordinate{x, y, z}))
                {
                    s1[c.z][c.x][c.y];
                }
            }
        }
    }

    return s1;
}

auto cycle(const space& s) -> space
{
    auto next = s;

    for(const auto& [z, zdata] : s)
    {
        for(const auto& [x, xdata] : zdata)
        {
            for(const auto& [y, state] : xdata)
            {
                uint64_t active_neighbors{0};

                for(const auto& c : generate_neighbors(coordinate{x, y, z}))
                {
                    auto z_iter = s.find(c.z);
                    if(z_iter != s.end())
                    {
                        const auto& [z_prime, zdata_prime] = *z_iter;

                        auto x_iter = zdata_prime.find(c.x);
                        if(x_iter != zdata_prime.end())
                        {
                            const auto& [x_prime, xdata_prime] = *x_iter;

                            auto y_iter = xdata_prime.find(c.y);
                            if(y_iter != xdata_prime.end())
                            {
                                const auto& [y_prime, state_prime] = *y_iter;

                                if(state_prime == state::active)
                                {
                                    ++active_neighbors;
                                }
                            }
                        }
                    }
                }

                std::cout << "[" << z << ", " << x << ", " << y << "] is " << to_string(state) << " and has " << active_neighbors << " ";

                switch(state)
                {
                    case state::active:
                        if(active_neighbors >= 2 && active_neighbors <= 3)
                        {
                            next[z][x][y] = state::active;
                            std::cout << " remaining active\n";
                        }
                        else
                        {
                            next[z][x][y] = state::inactive;
                            std::cout << " going inactive\n";
                        }
                        break;
                    case state::inactive:
                        if(active_neighbors == 3)
                        {
                            next[z][x][y] = state::active;
                            std::cout << " going active\n";
                        }
                        else
                        {
                            next[z][x][y] = state::inactive;
                            std::cout << " remaining inactive\n";
                        }
                        break;
                }
            }
        }
    }

    return next;
}

auto active(const space& s) -> uint64_t
{
    uint64_t active{0};
    for(const auto& [z, zdata] : s)
    {
        for(const auto& [x, xdata] : zdata)
        {
            for(const auto& [y, state] : xdata)
            {
                if(state == state::active)
                {
                    ++active;
                }
            }
        }
    }
    return active;
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

    space s{};

    size_t x{0};
    for(const auto& line : chain::str::split(contents, '\n'))
    {
        size_t y{0};
        for(const auto c : line)
        {
            s[0][x][y] = (c == '#') ? state::active : state::inactive;
            s[-1][x][y] = state::inactive;
            s[1][x][y] = state::inactive;
            ++y;
        }
        ++x;
    }


    std::cout << "Before any cycles:\n\n";
    std::cout << s << "\n";

    // This is important to guarantee its wide enough from the start.
    s = expand(s);

    for(size_t i = 0; i < 6; ++i)
    {
        s = cycle(s);

        std::cout << "After " << i << " cycles:\n\n";
        std::cout << s << "\n";

        s = expand(s);
    }

    std::cout << "Total active cubes = " << active(s) << "\n";

    return 0;
}
