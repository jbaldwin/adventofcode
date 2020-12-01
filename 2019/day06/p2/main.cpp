#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>

#include <chain/chain.hpp>

struct Orbit
{
    std::string name{};
    std::string orbits{};
    std::set<std::string> orbitees{};
};

using OrbitType = std::map<std::string, Orbit>;

auto cfind(const OrbitType& orbits, const std::string& name) -> const Orbit&
{
    if(const auto exists = orbits.find(name); exists != orbits.end())
    {
        return exists->second;
    }

    throw std::runtime_error("impossible");
}

auto count_transfers(
    const OrbitType& orbits,
    const Orbit& current,
    const std::string& find,
    const std::string& parent,
    std::size_t depth = 0
) -> std::optional<std::size_t>
{
    if(current.name == find)
    {
        return {depth};
    }

    for(const auto& orbitee : current.orbitees)
    {
        // don't revisit your parent...!
        if(orbitee != parent)
        {
            const auto& next_orbit = cfind(orbits, orbitee);
            auto path_found = count_transfers(
                    orbits,
                    next_orbit,
                    find,
                    current.name,
                    depth + 1);
            if(path_found.has_value())
            {
                return path_found;
            }
        }
    }

    if(current.orbits != parent)
    {
        const auto& next_orbit = cfind(orbits, current.orbits);
        auto path_found = count_transfers(
                orbits,
                next_orbit,
                find,
                current.name,
                depth + 1);
        if(path_found.has_value())
        {
            return path_found;
        }
    }

    return {};
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};

    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream input_file{args[1]};
    std::string line{};

    std::map<std::string, Orbit> orbits{};

    while(std::getline(input_file, line))
    {
        auto parts = chain::str::split(line, ')');
        if(parts.size() != 2)
        {
            std::cerr << "Malformed input: " << line << std::endl;
        }

        auto parent_name = std::string{parts[0]};
        auto self_name = std::string{parts[1]};

        auto& parent = orbits[parent_name];
        auto& self   = orbits[self_name];

        self.name = self_name;
        self.orbits = std::move(parent_name);
        parent.orbitees.insert(std::move(self_name));
    }

    const auto& you_orbit = orbits["YOU"];

    const auto transfers = count_transfers(
        orbits, 
        you_orbit,
        "SAN",
        you_orbit.name);
    
    // The problem doesn't want transfer between YOU and SAN, it wants orbital
    // transfers so you are orbiting the same object... so subtract 2 transfers off.
    std::cout << transfers.value() - 2 << std::endl;

    return 0;
}
