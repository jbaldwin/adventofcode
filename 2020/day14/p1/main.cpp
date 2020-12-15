#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <array>
#include <map>
#include <bitset>

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

            auto parts = chain::str::split(line, '=');
            auto value = chain::str::to_number<uint64_t>(chain::str::trim_view(parts[1])).value();
            auto value_str = std::bitset<36>(value).to_string();

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
                    result[i] = '0';
                }
                else
                {
                    result[i] = value_str[i];
                }
            }

            uint64_t result_d = std::bitset<36>(result).to_ulong();

            std::cout << "value:  " << value_str << "   (decimal " << value << ")\n";
            std::cout << "mask:   " << mask << "\n";
            std::cout << "result: " << result << "   (decimal " << result_d << ")\n\n";

            memory[addr] = result_d;
        }
    }

    // ranged based for loop sum is less code, but here we are, figuring out that
    // the intial value type has to be correct otherwise accumulate's template parameters get
    // inferred incorrectly and then you get a nasty overflow, just c++ things.
    auto sum = std::accumulate(
        memory.begin(),
        memory.end(),
        0ul, // muy importante
        [](auto acc, const auto& p) { return acc + p.second; }
    );

    std::cout << "sum = " << sum << "\n";

    return 0;
}
