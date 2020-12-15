#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <set>
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

    std::set<char> group{};

    for(const auto& line : chain::str::split(contents, '\n'))
    {
        if(line.empty())
        {
            total += group.size();
            group.clear();
            continue;
        }

        for(const auto& c : line)
        {
            group.emplace(c);
        }
    }

    std::cout << "What is the sum of those counts? " << total << "\n";

    return 0;
}
