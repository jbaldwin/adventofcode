#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

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
    auto medicine_molecule = parts[1];

    std::multimap<std::string, std::string> replacements{};
    for(const auto& line : chain::str::split(replacement_inputs_all, '\n'))
    {
        auto replacement_parts = chain::str::split(line, " => ");
        replacements.emplace(
            std::string{replacement_parts[0]},
            std::string{replacement_parts[1]}
        );
    }

    std::set<std::string> molecules{};
    for(const auto& [from, to] : replacements)
    {
        std::cout << from << " => " << to << '\n';

        std::size_t pos{0};
        while(true)
        {
            if(auto found = medicine_molecule.find(from, pos); found != std::string::npos)
            {
                auto generated = std::string{medicine_molecule};
                generated.replace(found, from.length(), to);
                molecules.emplace(std::move(generated));

                pos = found + 1;
            }
            else
            {
                break;
            }
        }
    }

    for(const auto& m : molecules)
    {
        std::cout << m << '\n';
    }
    std::cout << std::size(molecules) << '\n';

    return 0;
}
