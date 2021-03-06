#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
#include <optional>
#include <map>

#include <lib/file_util.hpp>
#include <chain/chain.hpp>
#include <lib/algorithms.hpp>

enum class OpCodeEnum
{
    ADD = 1,
    MUL = 2,
    INPUT = 3,
    OUTPUT = 4,
    JUMP_IF_TRUE = 5,
    JUMP_IF_FALSE = 6,
    LESS_THAN = 7,
    EQUALS = 8,
    END = 99
};

enum class ParameterModeEnum
{
    POSITION = 0,
    IMMEDIATE = 1
};

auto read_instructions(std::string_view file_name) -> std::vector<int64_t>
{
    auto raw_instructions = file::read(file_name);
    auto split_instructions = chain::str::split(raw_instructions, ',');

    std::vector<int64_t> instructions{};
    instructions.reserve(split_instructions.size());

    for (const auto &inst_str_view : split_instructions)
    {
        instructions.push_back(std::stol(std::string{inst_str_view}));
    }

    return instructions;
}

auto parse_op_code(const std::string& instruction) -> OpCodeEnum
{
    if(instruction.length() == 1 || instruction.length() == 2)
    {
        return static_cast<OpCodeEnum>(std::stol(instruction));
    }
    else
    {
        return static_cast<OpCodeEnum>(std::stol(instruction.substr(instruction.length() - 2)));
    }
}

/**
 * @param instruction The stringified instruction string.
 * @param param_idx The parameter index, e.g. parameter 1, 2 or 3.
 * @return The mode the parameter should be interpreted as.
 **/
auto parse_parameter_mode(
    const std::string& instruction,
    std::size_t param_idx
) -> ParameterModeEnum
{
    // This instruction does not have any parameter modes, use default.
    if(instruction.length() <= 2)
    {
        return ParameterModeEnum::POSITION;
    }

    std::size_t inst_idx = instruction.length() - 2 - param_idx;

    // If the parameter index is within the instructions bounds
    if(inst_idx >= 0)
    {
        if(instruction[inst_idx] == '1')
        {
            return ParameterModeEnum::IMMEDIATE;
        }
    }

    return ParameterModeEnum::POSITION;
}

auto exec_load_param(
    const std::vector<int64_t>& instructions,
    int64_t inst_ptr,
    ParameterModeEnum mode
) -> int64_t
{
    return (mode == ParameterModeEnum::POSITION)
        ? instructions[instructions[inst_ptr]]
        : instructions[inst_ptr];
}

auto exec_store(
    std::vector<int64_t>& instructions,
    int64_t inst_ptr,
    int64_t value
) -> void
{
    auto store_at = instructions[inst_ptr];
    instructions[store_at] = value;
}

auto execute(
    std::vector<int64_t>& instructions,
    int64_t input1,
    int64_t input2
) -> std::optional<int64_t>
{
    bool input_1_used = false;

    std::optional<int64_t> output{};

    int64_t inst_ptr = 0;

    while (true)
    {
        using Mode = ParameterModeEnum;

        const auto i = instructions[inst_ptr];
        const auto i_str = std::to_string(i);

        auto op_code = parse_op_code(i_str);

        auto p1_mode = parse_parameter_mode(i_str, 1);
        auto p2_mode = parse_parameter_mode(i_str, 2);

        switch(op_code)
        {
        case OpCodeEnum::ADD:
        {
            // Opcode 1 adds together numbers read from two positions and stores the result
            // in a third position.  The three integers immediately after the opcode tell
            // you these three positions - the first two indicate the positions from which
            // you should read the input values, and the third indicates the position at
            // which the output should be stored.
            auto val1  = exec_load_param(instructions, inst_ptr + 1, p1_mode);
            auto val2  = exec_load_param(instructions, inst_ptr + 2, p2_mode);
            exec_store(instructions, inst_ptr + 3, val1 + val2);
            inst_ptr += 4;
        }
        break;
        case OpCodeEnum::MUL:
        {
            // Opcode 2 works exactly like opcode 1, except it mutiples the two inputs
            // instead of adding them.
            auto val1  = exec_load_param(instructions, inst_ptr + 1, p1_mode);
            auto val2  = exec_load_param(instructions, inst_ptr + 2, p2_mode);
            exec_store(instructions, inst_ptr + 3, val1 * val2);
            inst_ptr += 4;
        }
        break;
        case OpCodeEnum::INPUT:
        {
            // Opcode 3 takes a single integer as input and saves it to the position
            // given by its only parameter.  For example, the instruciton 3,50 would
            // take an input value and store it at address 50.
            auto input = (input_1_used) ? input2 : input1;
            input_1_used = true;
            exec_store(instructions, inst_ptr + 1, input);
            inst_ptr += 2;
        }
        break;
        case OpCodeEnum::OUTPUT:
        {
            // Opcode 4 outputs the value of its only parameter.  For example, the
            // instruction 4,50 would output the value at address 50.
            output = exec_load_param(instructions, inst_ptr + 1, p1_mode);
            inst_ptr += 2;
        }
        break;
        case OpCodeEnum::JUMP_IF_TRUE:
        {
            // Opcode 5 is jump-if-true: If the first parameter is non-zero, it sets 
            // the instruction pointer to the value from the second parameter.
            // Otherwise, it does nothing.
            auto param1 = exec_load_param(instructions, inst_ptr + 1, p1_mode);

            if(param1 != 0)
            {
                inst_ptr = exec_load_param(instructions, inst_ptr + 2, p2_mode);
            }
            else
            {
                inst_ptr += 3;
            }
        }
        break;
        case OpCodeEnum::JUMP_IF_FALSE:
        {
            // Opcode 6 is jump-if-false: If the first parameter is zero, it sets the
            // instruction pointer to the value from the second parameter.  Othewrise,
            // it does nothing.
            auto param1 = exec_load_param(instructions, inst_ptr + 1, p1_mode);

            if(param1 == 0)
            {
                inst_ptr = exec_load_param(instructions, inst_ptr + 2, p2_mode);
            }
            else
            {
                inst_ptr += 3;
            }
        }
        break;
        case OpCodeEnum::LESS_THAN:
        {
            // Opcode 7 is less than: if the first parameter is less than the second
            // parameter, it stores 1 in the position given by the third parameter.
            // Otherwise, it stores 0.
            auto param1 = exec_load_param(instructions, inst_ptr + 1, p1_mode);
            auto param2 = exec_load_param(instructions, inst_ptr + 2, p2_mode);
            exec_store(instructions, inst_ptr + 3, (param1 < param2));
            inst_ptr += 4;
        }
        break;
        case OpCodeEnum::EQUALS:
        {
            // Opcode 8 is equals: if the first parameter is equal to the second
            // parameter, it stores 1 in the position given by the third parameter.
            // Otherwise, it stores 0.
            auto param1 = exec_load_param(instructions, inst_ptr + 1, p1_mode);
            auto param2 = exec_load_param(instructions, inst_ptr + 2, p2_mode);
            exec_store(instructions, inst_ptr + 3, (param1 == param2));
            inst_ptr += 4;
        }
        break;
        case OpCodeEnum::END:
        {
            return output;
        }
        break;
        default:
        {
            std::cerr << "Unknown opcode: " << i << " exiting.";
            return {};
        }
        break;
        }
    }

    return {};
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <instruction_file>" << std::endl;
        return 1;
    }

    auto instructions = read_instructions(args[1]);

    std::map<std::string, int64_t> results{};

    algo::permutate<std::array<int64_t, 5> >(
        std::array<int64_t, 5>{0,1,2,3,4},
        [&results, &instructions](const std::array<int64_t, 5> p) -> void
        {
            std::string key{};
            key.reserve(5);

            int64_t input = 0;

            for(const auto& phase_setting : p)
            {
                key += std::to_string(phase_setting);

                auto copy = instructions;

                input = execute(copy, phase_setting, input).value();
            }

            results.emplace(std::move(key), input);
        }
    );

    std::size_t max_thruster = 0;
    std::string max_signal;

    for(const auto& [signal, thruster_value] : results)
    {
        if(thruster_value > max_thruster)
        {
            max_thruster = thruster_value;
            max_signal = signal;
        }
    }

    std::cout << "signal " << max_signal << " thruster " << max_thruster << std::endl;

    return 0;
}