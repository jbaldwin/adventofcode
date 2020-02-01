#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>

#include <lib/FileUtil.h>
#include <lib/StringUtil.h>
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

    friend auto operator<<(std::ostream& os, const Moon& m) -> std::ostream&
    {
        os << "{";
        os << "pos=[";
        os << "x=" << std::setw(5) << m.pos.x;
        os << ", y=" << std::setw(5) << m.pos.y;
        os << ", z=" << std::setw(5) << m.pos.z;
        os << "]";
        os << ", vel[";
        os << "x=" << std::setw(5) << m.vel.x;
        os << ", y=" << std::setw(5) << m.vel.y;
        os << ", z=" << std::setw(5) << m.vel.z;
        os << "]";
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
    auto serialized_moons = str::split(positions_contents, '\n');

    for(auto smoon : serialized_moons)
    {
        if(smoon.empty())
        {
            continue;
        }

        // remove < and >
        smoon.remove_prefix(1);
        smoon.remove_suffix(1);

        auto serialized_xyz = str::split(smoon, ',');

        auto x_split = str::split(serialized_xyz[0], '=');
        auto y_split = str::split(serialized_xyz[1], '=');
        auto z_split = str::split(serialized_xyz[2], '=');
        
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

auto step(
    std::vector<Moon>& moons
) -> void
{
    // First update all velocities
    for(size_t i = 0; i < moons.size(); ++i)
    {
        auto& m = moons[i];

        Vec3 g{};
        for(const auto& m2 : moons)
        {
            g.x += gravity(m.pos.x, m2.pos.x);
            g.y += gravity(m.pos.y, m2.pos.y);
            g.z += gravity(m.pos.z, m2.pos.z);
        }

        m.vel.x += g.x;
        m.vel.y += g.y;
        m.vel.z += g.z;
    }

    // Next update all positions.
    for(auto& m : moons)
    {
        m.pos.x += m.vel.x;
        m.pos.y += m.vel.y;
        m.pos.z += m.vel.z;
    }
}

auto energy(
    const std::vector<Moon>& moons
) -> int64_t
{
    int64_t e{0};

    for(const auto& m : moons)
    {
        int64_t p_e = std::abs(m.pos.x) + std::abs(m.pos.y) + std::abs(m.pos.z);
        int64_t k_e = std::abs(m.vel.x) + std::abs(m.vel.y) + std::abs(m.vel.z);

        e += p_e * k_e;
    }

    return e;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 3)
    {
        std::cout << args[0] << " <positions_file> <steps>" << std::endl;
        return 1;
    }

    auto moons = load_moons(args[1]);
    auto steps = std::stol(args[2]);

    std::cout << "After 0 steps:" << std::endl;
    for(const auto& m : moons)
    {
        std::cout << m << std::endl;
    }

    for(int64_t s = 1; s <= steps; ++s)
    {
        step(moons);

        std::cout << "After " << s << " steps:" << std::endl;
        for(const auto& m : moons)
        {
            std::cout << m << std::endl;
        }
    }

    std::cout << "Energy: " << energy(moons) << std::endl;

    return 0;
}
