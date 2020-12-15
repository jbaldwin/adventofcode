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
    auto lines = chain::str::split(contents, '\n');

    std::map<uint64_t, uint64_t> memory{};
    std::string mask;

    for(auto& line : lines)
    {
        if(chain::str::starts_with(line, "mask = "))
        {
            line.remove_prefix(7);
            mask = std::string{line};
        }
        else
        {
            line.remove_prefix(4);
            auto addr = chain::str::to_number<uint64_t>(line).value();
            auto addr_str = std::bitset<36>(addr).to_string();

            auto parts = chain::str::split(line, '=');
            auto value = chain::str::to_number<uint64_t>(chain::str::trim_view(parts[1])).value();

            size_t xs{0};

            std::string result;
            result.resize(36);
            for(size_t i = 0; i < mask.size(); ++i)
            {
                if(mask[i] == '1')
                {
                    result[i] = '1';
                }
                else if(mask[i] == '0')
                {
                    result[i] = addr_str[i];
                }
                else // 'X'
                {
                    result[i] = 'X';
                    ++xs;
                }
            }

            uint64_t max = std::pow(2ul, xs);

            for(uint64_t i = 0; i < max; ++i)
            {
                auto address = result;
                auto combination = std::bitset<36>(i).to_string();

                size_t ci{0};
                size_t ai{0};
                while(ci < xs)
                {
                    while(address[ai] != 'X')
                    {
                        ++ai;
                    }

                    address[ai] = combination[35 - ci];
                    ++ci;
                }

                auto addr_d = std::bitset<36>(address).to_ulong();
                memory[addr_d] = value;
            }
        }
    }

    auto sum = std::accumulate(
        memory.begin(),
        memory.end(),
        0ul, // muy importante
        [](auto acc, const auto& p) { return acc + p.second; }
    );

    std::cout << "sum = " << sum << "\n";

    return 0;
}
