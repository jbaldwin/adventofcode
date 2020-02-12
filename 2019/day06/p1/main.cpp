#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>

#include <lib/StringUtil.h>

struct Orbit
{
    std::string orbits{};
    std::set<std::string> orbitees{};
};

using OrbitType = std::map<std::string, Orbit>;

auto count_orbits(
    const OrbitType& orbits,
    const Orbit& current,
    std::size_t depth = 0
) -> std::pair<std::size_t, std::size_t>
{
    std::size_t direct   = 1;         // All orbits have 1 direct.
    std::size_t indirect = depth - 1; // This node has depth - 1 indirect orbits

    for(const auto& orbitee : current.orbitees)
    {
        if(const auto exists = orbits.find(orbitee); exists != orbits.end())
        {
            const auto [d, i] = count_orbits(orbits, exists->second, depth + 1);
            direct   += d;
            indirect += i;
        }
    }
    
    return {direct, indirect};
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
        auto parts = str::split(line, ')');
        if(parts.size() != 2)
        {
            std::cerr << "Malformed input: " << line << std::endl;
        }

        auto parent_name = std::string{parts[0]};
        auto self_name = std::string{parts[1]};

        auto& parent = orbits[parent_name];
        auto& self   = orbits[self_name];

        parent.orbitees.insert(std::move(self_name));
        self.orbits = std::move(parent_name);
    }

    const auto [direct, indirect] = count_orbits(orbits, orbits["COM"]);
    
    std::cout << "Direct orbits: " << direct << std::endl;
    std::cout << "Indirect orbirts: " << indirect << std::endl;
    std::cout << "Total orbits: " << (direct + indirect) << std::endl;

    return 0;
}
