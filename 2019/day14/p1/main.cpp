#include <iostream>
#include <vector>
#include <map>

#include <lib/FileUtil.h>
#include <chain/chain.hpp>

struct Reaction
{
    std::string name{};
    uint64_t amount{0};

    std::map<std::string, uint64_t> inputs{};

    friend auto operator<<(std::ostream& os, const Reaction& r) -> std::ostream&
    {
        os << "{";
        os << r.name << ":" << r.amount;
        os << "[";

        os << chain::str::map_join(
            r.inputs,
            ",",
            [](const auto& input) {
                return input.first + ":" + std::to_string(input.second);
            }
        );
        os << "]";
        os << "}";

        return os;
    }
};

auto dependencies(
    std::map<std::string, Reaction>& reactions,
    std::map<std::string, std::pair<uint64_t, uint64_t>>& reqs,
    const std::string& current,
    uint64_t needed
) -> void
{
    // Treat ORE specially, since you can produce infinite amounts of it.
    if(current == "ORE")
    {
        reqs[current].first += needed;
    }
    else
    {
        // Produce inputs to this current node until current available
        // is greater or equal to the needed amount from the parent reaction.
        const auto& curr_r = reactions[current];
        auto& [curr_used, curr_available] = reqs[current];

        while(curr_available < needed)
        {
            curr_available += curr_r.amount;
            for(const auto& [child_name, child_needed]: curr_r.inputs)
            {
                dependencies(reactions, reqs, child_name, child_needed);
            }
        }

        curr_available -= needed;
        curr_used += needed;
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
    std::map<std::string, Reaction> reactions{};

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
        auto o_amount = std::stoul(std::string{output_parts[0]});
        auto o_name = std::string{output_parts[1]};

        r.name = std::move(o_name);
        r.amount = o_amount;

        auto input_parts = chain::str::split(left_right[0], ", ");
        for(const auto& input_part : input_parts)
        {
            auto lr = chain::str::split(input_part, ' ');
            auto i_amount = std::stoul(std::string{lr[0]});
            auto i_name = std::string{lr[1]};

            r.inputs.emplace(std::move(i_name), i_amount);
        }

        reactions.emplace(r.name, std::move(r));
    }

    // name => {used, available}
    std::map<std::string, std::pair<uint64_t, uint64_t>> reqs{};

    dependencies(reactions, reqs, "FUEL", 1);

    std::cout << "Required ORE=" << reqs["ORE"].first << std::endl;

    return 0;
}
