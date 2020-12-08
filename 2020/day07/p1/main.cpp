#include <lib/FileUtil.h>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <queue>
#include <set>

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);

    std::map<std::string, std::vector<std::pair<std::string, uint64_t>>> relations{};

    for(auto& line : chain::str::split(contents, '\n'))
    {
        auto parts = chain::str::split(line, " bags contain ");
        auto bag_type = std::string{parts[0]};
        auto contain_parts = chain::str::split(parts[1], ',');

        auto& contain_types = relations[bag_type];
        for(auto& cpart : contain_parts)
        {
            if(chain::str::ends_with(cpart, "."))
            {
                cpart.remove_suffix(1);
            }

            if(chain::str::ends_with(cpart, " bag"))
            {
                cpart.remove_suffix(4);
            }
            else // " bags"
            {
                cpart.remove_suffix(5);
            }

            cpart = chain::str::trim_left_view(cpart);

            if(cpart != "no other")
            {
                auto amount = chain::str::to_number<uint64_t>(cpart).value();
                auto first_space_pos = cpart.find(' ');
                cpart.remove_prefix(first_space_pos + 1);

                contain_types.emplace_back(std::string{cpart}, amount);
            }
        }
    }

    // Reverse the mapping to make it easier to traverse the graph.
    std::map<std::string, std::set<std::string>> reverse{};
    for(const auto& [bag_type, contains_type] : relations)
    {
        for(const auto& [ctype, amount] : contains_type)
        {
            reverse[ctype].insert(bag_type);
        }
    }

    // Create a queue of bag types to traverse that can contain the current
    // bag being inspected.
    std::deque<std::string> bags{};
    // Create a set of bags that can contain the starting bag type up the parent
    // bag chain.  This also serves to stop duplicate paths.
    std::set<std::string> visited{};

    bags.push_back("shiny gold");

    while(!bags.empty())
    {
        auto bag = bags.front();
        bags.pop_front();

        // If this bag type hasn't been visited before, traverse.
        if(visited.insert(bag).second)
        {
            std::cout << bag << "\n";
            for(const auto& parent_bag : reverse[bag])
            {
                bags.push_back(parent_bag);
            }
        }
    }

    // Don't count the shiny gold bag itself!
    std::cout << (visited.size() - 1) << " can contain a shiny gold bag\n";

    return 0;
}
