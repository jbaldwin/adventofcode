#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>

#include <lib/FileUtil.h>
#include <chain/chain.hpp>

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    auto contents = file::read(args[1]);

    auto parts = chain::str::split(contents, "\n\n");
    auto replacement_inputs_all = parts[0];
    auto molecule = std::string{parts[1]};

    std::vector<std::pair<std::string, std::string>> replacements{};

    for(const auto& line : chain::str::split(replacement_inputs_all, '\n'))
    {
        auto replacement_parts = chain::str::split(line, " => ");
        replacements.emplace_back(
            std::string{replacement_parts[0]},
            std::string{replacement_parts[1]}
        );
    }

    auto target = molecule;
    std::size_t count{0};

    while(target != "e")
    {
        auto prev = target;

        for(const auto& [to, from] : replacements)
        {
            while(true)
            {
                if(auto found = target.find(from); found != std::string::npos)
                {
                    target.replace(found, from.length(), to);
                    count++;
                }
                else
                {
                    break;
                }
            }
        }

        // If we end up where we started we need to try again with a different
        // ordering of the replacements.
        if(prev == target)
        {
            target = molecule;
            count = 0;

            static std::random_device rd{};
            static std::mt19937 g{rd()};
            std::shuffle(replacements.begin(), replacements.end(), g);
        }
    }

    std::cout << count << "\n";

    return 0;
}
