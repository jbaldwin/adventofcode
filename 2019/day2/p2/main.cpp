#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>

#include "../../../lib/FileUtil.h"
#include "../../../lib/StringUtil.h"

enum class OpCodeEnum
{
    ADD = 1,
    MUL = 2,
    END = 99
};

auto read_instructions(std::string_view file_name) -> std::vector<uint64_t>
{
    auto raw_instructions = file::read(file_name);
    auto split_instructions = str::split(raw_instructions, ',');

    std::vector<uint64_t> instructions{};
    instructions.reserve(split_instructions.size());

    for (const auto &inst_str_view : split_instructions)
    {
        instructions.push_back(std::stoul(std::string{inst_str_view}));
    }

    return instructions;
}

/**
 * @returns {store_location, value1, value2}
 **/
auto load(const std::vector<uint64_t>& instructions, uint64_t inst_ptr) -> std::tuple<uint64_t, uint64_t, uint64_t>
{
    auto load1 = instructions[inst_ptr + 1];
    auto load2 = instructions[inst_ptr + 2];
    auto store = instructions[inst_ptr + 3];

    auto val1 = instructions[load1];
    auto val2 = instructions[load2];

    return {store, val1, val2};
}

auto execute(std::vector<uint64_t>& instructions) -> void
{
    uint64_t inst_ptr = 0;

    while (true)
    {
        switch (instructions[inst_ptr])
        {
        case static_cast<uint64_t>(OpCodeEnum::ADD):
        {
            const auto &[store, val1, val2] = load(instructions, inst_ptr);
            instructions[store] = val1 + val2;
        }
        break;
        case static_cast<uint64_t>(OpCodeEnum::MUL):
        {
            const auto &[store, val1, val2] = load(instructions, inst_ptr);
            instructions[store] = val1 * val2;
        }
        break;
        case static_cast<uint64_t>(OpCodeEnum::END):
        {
            return;
        }
        break;
        default:
            std::cerr << "Unknown opcode: " << instructions[inst_ptr] << " exiting.";
            return;
        }

        inst_ptr += 4;
    }
}

int main(int argc, char* argv[])
{
    auto instructions = read_instructions("../p1/input.txt");

    for (uint64_t noun = 0; noun <= 99; ++noun)
    {
        for (uint64_t verb = 0; verb <= 99; ++verb)
        {
            auto copy = instructions;

            copy[1] = noun;
            copy[2] = verb;

            execute(copy);

            if (copy[0] == 19690720)
            {
                std::cout << (100 * noun + verb) << std::endl;
                return 0;
            }
        }
    }

    return 0;
}
