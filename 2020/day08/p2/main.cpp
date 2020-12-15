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

    // The current index that is attempting to be swapped to find the right solution.
    size_t swap_idx{0};

    // Auto swap the current instruction if applicable.
    struct swapper
    {
        swapper(instruction& i) : inst(i)
        {
            swapped = swap(inst);
        }
        ~swapper()
        {
            swap(inst);
        }

        bool swapped{false};
        instruction& inst;
    };

    while(true)
    {
        swapper s{instructions[swap_idx++]};
        if(!s.swapped)
        {
            continue;
        }

        std::vector<bool> executed(lines.size(), false);
        int64_t accumulator{0};
        int64_t inst_ptr{0};

        bool failed{true};
        while(inst_ptr >= 0 && inst_ptr <= instructions.size() && !executed[inst_ptr])
        {
            if(inst_ptr == instructions.size())
            {
                failed = false;
                std::cout << "swap_idx = " << (swap_idx - 1) << "\n";
                break;
            }

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

        if(!failed)
        {
            std::cout << "accumulator = " << accumulator << "\n";
            break;
        }
    }

    return 0;
}
