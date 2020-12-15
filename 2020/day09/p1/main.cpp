#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <queue>

enum class instruction_type
{
    acc,
    jmp,
    nop
};

struct instruction
{
    instruction_type type{instruction_type::nop};
    int64_t value{0};
};

auto swap(instruction& i) -> bool
{
    switch(i.type)
    {
        case instruction_type::jmp:
            i.type = instruction_type::nop;
            return true;
        case instruction_type::nop:
            i.type = instruction_type::jmp;
            return true;
        default:
            return false;
            break;
    }
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 3)
    {
        std::cout << args[0] << " <input_file> <preamble_length" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);
    auto lines = chain::str::split(contents, '\n');

    uint64_t preamble_length = chain::str::to_number<uint64_t>(args[2]).value();

    std::deque<uint64_t> values{};

    for(size_t i = 0; i < preamble_length; ++i)
    {
        values.emplace_back(chain::str::to_number<uint64_t>(lines[i]).value());
    }

    for(size_t i = preamble_length; i < lines.size(); ++i)
    {
        const auto value = chain::str::to_number<uint64_t>(lines[i]).value();

        bool fits = [&]() {
            for(const auto& a : values)
            {
                for(const auto& b : values)
                {
                    if(a != b && a + b == value)
                    {
                        return true;
                    }
                }
            }
            return false;
        }();

        if(!fits)
        {
            std::cout << value << " does not meet the properties requirements\n";
        }

        values.pop_front();
        values.push_back(value);
    }

    return 0;
}
