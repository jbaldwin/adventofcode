#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

#include <lib/FileUtil.h>
#include <chain/Chain.hpp>
#include <lib/Algorithms.hpp>

struct Person
{
    std::map<std::string, int64_t> happiness{};
};

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    auto contents = file::read(args[1]);
    auto lines = chain::str::split(contents, '\n');

    std::set<std::string> unique_names{};
    std::map<std::string, Person> people{};

    for(const auto& line : lines)
    {
        if(line.empty())
        {
            continue;
        }

        auto parts = chain::str::split(line, ' ');

        auto who = std::string{parts[0]};
        auto action = parts[2];
        auto amount = std::stol(std::string{parts[3]});
        parts[10].remove_suffix(1); // drop '.'
        auto next_to = std::string{parts[10]};

        if(action == "lose")
        {
            amount = -amount;
        }

        unique_names.emplace(who);
        people[who].happiness.emplace(std::move(next_to), amount);
    }

    std::vector<std::string> names{};
    names.reserve(people.size());
    for(const auto& [name, person] : people)
    {
        names.emplace_back(name);

        people["Santa"].happiness[name] = 0;
        people[name].happiness["Santa"] = 0;
    }

    int64_t max_happiness{0};
    algo::permutate<std::vector<std::string>>(
        names,
        [&people, &max_happiness](const std::vector<std::string>& n) -> void 
        {
            int64_t happiness{0};

            for(std::size_t i = 0; i < n.size(); ++i)
            {
                const auto& curr = n[i];
                std::string prev{};
                std::string next{};

                if(i == 0)
                {
                    prev = n.back();
                    next = n[i + 1];
                }
                else if(i == n.size() - 1)
                {
                    prev = n[i - 1];
                    next = n.front();
                }
                else
                {
                    prev = n[i - 1];
                    next = n[i + 1];
                }

                happiness += people[curr].happiness[prev];
                happiness += people[curr].happiness[next];
            }

            max_happiness = std::max(max_happiness, happiness);
        }
    );

    std::cout << max_happiness << '\n';

    return 0;
}
