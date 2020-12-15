#include <lib/file_util.hpp>
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

    size_t bag_count{0};
    std::deque<std::string> bags{};

    bags.push_back("shiny gold");

    while(!bags.empty())
    {
        auto bag = bags.front();
        bags.pop_front();

        for(const auto& [ctype, amount] : relations[bag])
        {
            bag_count += amount;
            for(size_t i = 0; i < amount; ++i)
            {
                bags.emplace_back(ctype);
            }
        }
    }

    std::cout << "A single shiny gold bag must contain " << bag_count << " bags.\n";

    return 0;
}
