#include <iostream>
#include <vector>
#include <map>

#include <lib/FileUtil.h>
#include <chain/chain.hpp>

struct Reaction
{
    std::string name{};
    uint64_t produced{0};

    uint64_t used{0};
    uint64_t available{0};

    // input name => needed amount
    std::map<std::string, uint64_t> inputs{};
};

using Reactions = std::map<std::string, Reaction>;

auto dependencies(
    Reactions& reactions,
    const std::string& current,
    uint64_t needed
) -> void
{
    // Treat ORE specially, since you can produce infinite amounts of it.
    if(current == "ORE")
    {
        auto& ore = reactions[current];
        if(ore.available >= needed)
        {
            ore.available -= needed;
            ore.used += needed;
        }
        else
        {
            throw std::runtime_error("out of ORE!");
        }
    }
    else
    {
        // Produce inputs to this current node until current available
        // is greater or equal to the needed amount from the parent reaction.
        auto& curr = reactions[current];

        while(curr.available < needed)
        {
            curr.available += curr.produced;
            for(const auto& [child_name, child_needed]: curr.inputs)
            {
                dependencies(reactions, child_name, child_needed);
            }
        }

        curr.available -= needed;
        curr.used += needed;
    }
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    // output name is the key
    Reactions reactions{};

    auto contents = file::read(args[1]);
    auto lines = chain::str::split(contents, '\n');

    for(const auto& line : lines)
    {
        if(line.empty())
        {
            continue;
        }

        Reaction r{};

        auto left_right = chain::str::split(line, " => ");
        if(left_right.size() != 2)
        {
            std::cerr << "Malformed input file on '=>' split. " << line << std::endl;
            return 1;
        }

        auto output_parts = chain::str::split(left_right[1], ' ');
        auto o_produced = std::stoul(std::string{output_parts[0]});
        auto o_name = std::string{output_parts[1]};

        r.name = std::move(o_name);
        r.produced = o_produced;

        auto input_parts = chain::str::split(left_right[0], ", ");
        for(const auto& input_part : input_parts)
        {
            auto lr = chain::str::split(input_part, ' ');
            auto i_needed = std::stoul(std::string{lr[0]});
            auto i_name = std::string{lr[1]};

            r.inputs.emplace(std::move(i_name), i_needed);
        }

        reactions.emplace(r.name, std::move(r));
    }

    constexpr uint64_t one_trillion = 1'000'000'000'000;

    reactions["ORE"].available = one_trillion;

    std::map<uint64_t, Reactions> cache{};

    try
    {
        // seems fast enough, completed in about 11 minutes on 1 core
        // could probably binary search/double fuel counts fairly easy.
        dependencies(reactions, "FUEL", 1);
    }
    catch(...)
    {

    }

    std::cout << "Fuel produced from 1 trillion ORE = " << reactions["FUEL"].used << std::endl;

    return 0;
}
