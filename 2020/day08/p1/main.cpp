#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>

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
    std::vector<instruction> instructions{};
    instructions.reserve(lines.size());

    for(auto& line : lines)
    {
        instruction_type type{instruction_type::nop};
        if(chain::str::starts_with(line, "acc"))
        {
            type = instruction_type::acc;
        }
        else if(chain::str::starts_with(line, "jmp"))
        {
            type = instruction_type::jmp;
        }

        line.remove_prefix(4); // strip type

        int64_t value = chain::str::to_number<int64_t>(line).value();

        instructions.emplace_back(type, value);
    }

    std::vector<bool> executed(lines.size(), false);

    int64_t accumulator{0};
    int64_t inst_ptr{0};

    while(inst_ptr < instructions.size() && !executed[inst_ptr])
    {
        executed[inst_ptr] = true;

        const instruction& inst = instructions[inst_ptr];

        switch(inst.type)
        {
            case instruction_type::acc:
                accumulator += inst.value;
                ++inst_ptr;
                break;
            case instruction_type::jmp:
                inst_ptr += inst.value;
                break;
            case instruction_type::nop:
                ++inst_ptr;
                break;
        }
    }

    std::cout << accumulator << "\n";

    return 0;
}
