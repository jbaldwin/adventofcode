#include <lib/file_util.hpp>
#include <chain/chain.hpp>
#include <lib/algorithms.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <bitset>
#include <cmath>

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);
    auto numbers_parts = chain::str::split(contents, ',');

    {
        std::map<uint64_t, uint64_t> spoken_numbers{};

        bool first{true};
        size_t turn{0};
        uint64_t turn_seen{0};
        uint64_t last{0};
        for(const auto& number : numbers_parts)
        {
            auto value = chain::str::to_number<uint64_t>(number).value();
            spoken_numbers.insert_or_assign(value, ++turn);
            last = value;

        }

        while(turn < 2020)
        {
            uint64_t next{0};
            ++turn;

            if(!first)
            {
                next = turn - turn_seen - 1;
            }

            auto [iter, emplaced] = spoken_numbers.emplace(next, turn);
            first = emplaced;
            // If this has been seen before, update its turn seen, but cache the old turn seen.
            if(!first)
            {
                turn_seen = iter->second;
                iter->second = turn;
            }

            last = next;
        }

        std::cout << "2020th number spoke is " << last << "\n";
    }

    {
        std::map<uint64_t, uint64_t> spoken_numbers{};

        bool first{true};
        size_t turn{0};
        uint64_t turn_seen{0};
        uint64_t last{0};
        for(const auto& number : numbers_parts)
        {
            auto value = chain::str::to_number<uint64_t>(number).value();
            spoken_numbers.insert_or_assign(value, ++turn);
            last = value;

        }

        while(turn < 30000000)
        {
            uint64_t next{0};
            ++turn;

            if(!first)
            {
                next = turn - turn_seen - 1;
            }

            auto [iter, emplaced] = spoken_numbers.emplace(next, turn);
            first = emplaced;
            // If this has been seen before, update its turn seen, but cache the old turn seen.
            if(!first)
            {
                turn_seen = iter->second;
                iter->second = turn;
            }

            last = next;
        }

        std::cout << "30000000th number spoke is " << last << "\n";
    }

    return 0;
}
