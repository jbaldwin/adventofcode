#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>

#include <lib/FileUtil.h>
#include <chain/chain.hpp>
#include <lib/Algorithms.hpp>
#include <lib/Containers.hpp>

using Vec3 = containers::Vec3<int64_t>;

struct Moon
{
    Moon(Vec3 positions, Vec3 velocities)
        : pos(std::move(positions))
        , vel(std::move(velocities))
    {

    }

    Vec3 pos;
    Vec3 vel;

    auto operator==(const Moon& other) const -> bool
    {
        return pos == other.pos && vel == other.vel;
    }

    friend auto operator<<(std::ostream& os, const Moon& m) -> std::ostream&
    {
        os << "{";
        os << "pos=" << m.pos;
        os << ", vel" << m.vel;
        os << "}";
        return os;
    }
};

auto load_moons(
    std::string_view positions_file
) -> std::vector<Moon>
{
    std::vector<Moon> moons{};

    auto positions_contents = file::read(positions_file);
    auto serialized_moons = chain::str::split(positions_contents, '\n');

    for(auto smoon : serialized_moons)
    {
        if(smoon.empty())
        {
            continue;
        }

        // remove < and >
        smoon.remove_prefix(1);
        smoon.remove_suffix(1);

        auto serialized_xyz = chain::str::split(smoon, ',');

        auto x_split = chain::str::split(serialized_xyz[0], '=');
        auto y_split = chain::str::split(serialized_xyz[1], '=');
        auto z_split = chain::str::split(serialized_xyz[2], '=');
        
        moons.emplace_back(
            Vec3{
                std::stol(std::string{x_split[1]}),
                std::stol(std::string{y_split[1]}),
                std::stol(std::string{z_split[1]})
            },
            Vec3{0, 0, 0}
        );
    }

    return moons;
}

auto gravity(
    int64_t a,
    int64_t b
) -> int64_t
{
    if(a == b) return 0;
    if(a < b) return 1;
    return -1;
}

auto calc_periods(
    std::vector<Moon>& moons
) -> Vec3
{
    auto orig_moons = moons;

    Vec3 periods{};

    auto find_period = [&](size_t col) -> void {
        while(true)
        {
            // First update all velocities
            for(size_t i = 0; i < moons.size(); ++i)
            {
                auto& m = moons[i];

                for(const auto& m2 : moons)
                {
                    m.vel[col] += gravity(m.pos[col], m2.pos[col]);
                }
            }

            // Next update all positions.
            for(auto& m : moons)
            {
                m.pos[col] += m.vel[col];
            }

            ++periods[col];

            // Could just compare the column... but its already fast enough.
            if(orig_moons == moons)
            {
                break;
            }
        }
    };

    find_period(0);
    find_period(1);
    find_period(2);

    return periods;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <positions_file>" << std::endl;
        return 1;
    }

    auto moons = load_moons(args[1]);

    // For part two recognize that x,y,z are all independent and each
    // period can be calculated separately.  These periods can then
    // be determined to converge at their least common multiple point.

    auto periods = calc_periods(moons);

    std::cout << "Periods=" << periods << std::endl;

    auto converge = algo::lcm(periods.x, periods.y, periods.z);

    std::cout << converge << std::endl;

    return 0;
}
