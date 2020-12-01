#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <set>
#include <vector>

#include <chain/chain.hpp>

using Graph = std::unordered_map<
    std::string,
    std::unordered_map<std::string, uint64_t>>;

auto traverse(
    const Graph& routes,
    std::set<std::string>& visited_cities,
    const std::string& city,
    uint64_t dist_so_far = 0) -> uint64_t
{
    auto found = routes.find(city);
    if(found != routes.end())
    {
        visited_cities.insert(city);

        std::set<uint64_t, std::greater<uint64_t>> distances{};
        for(const auto& [neighbor_city, dist] : found->second)
        {
            if(visited_cities.find(neighbor_city) == visited_cities.end())
            {
                uint64_t this_dist = traverse(
                    routes,
                    visited_cities,
                    neighbor_city,
                    dist_so_far + dist);

                distances.insert(this_dist);
            }
        }

        visited_cities.erase(city);

        if(distances.empty())
        {
            // If all cities have been visited, return the full distance.
            return dist_so_far;
        }
        else
        {
            // Otherwise return the longest distance from this city.
            return *distances.begin();
        }
    }

    return 0;
}

int start(std::vector<std::string> args)
{
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream input_file{args[1]};
    std::string line{};

    Graph routes{};

    std::set<std::string> cities{};

    while(std::getline(input_file, line))
    {
        auto parts = chain::str::split(line, ' ');

        auto from = std::string{parts[0]};
        auto to   = std::string{parts[2]};
        auto dist = std::stoul(std::string{parts[4]});

        routes[from][to] = dist;
        routes[to][from] = dist;

        cities.insert(from);
        cities.insert(to);
    }

    std::set<uint64_t, std::greater<uint64_t>> distances{};
    for(const auto& city : cities)
    {
        std::set<std::string> visisted_cities{};
        auto dist = traverse(routes, visisted_cities, city);

        distances.insert(dist);
    }

    std::cout << *distances.begin() << std::endl;

    return 0;
}

int main(int argc, char* argv[])
{
    return start({argv, argv + argc});
}
