#include <lib/FileUtil.h>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <numeric>

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);

    size_t total{0};

    size_t group_member_count{0};
    std::map<char, uint64_t> group{};

    for(const auto& line : chain::str::split(contents, '\n'))
    {
        if(line.empty())
        {
            total += std::accumulate(
                group.begin(),
                group.end(),
                0,
                [&](uint64_t value, const auto& p)
                {
                    return value + ((p.second == group_member_count) ? 1: 0);
                }
            );

            group_member_count = 0;
            group.clear();
            continue;
        }

        ++group_member_count;
        for(const auto& c : line)
        {
            group[c]++;
        }
    }

    std::cout << "What is the sum of those counts? " << total << "\n";

    return 0;
}
