#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <tuple>
#include <optional>

#include <lib/FileUtil.h>
#include <chain/chain.hpp>

enum class OpCodeEnum
{
    ADD = 1,
    MUL = 2,
    INPUT = 3,
    OUTPUT = 4,
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

auto four_inst(
    std::vector<int64_t>& instructions,
    int64_t inst_ptr,
    ParameterModeEnum p1_mode,
    ParameterModeEnum p2_mode
    ) -> std::tuple<int64_t, int64_t, int64_t>
{
    int64_t store = instructions[inst_ptr + 3];

    auto load1 = instructions[inst_ptr + 1];
    auto val1 = (p1_mode == ParameterModeEnum::POSITION) ? instructions[load1] : load1;

    auto load2 = instructions[inst_ptr + 2];
    auto val2 = (p2_mode == ParameterModeEnum::POSITION) ? instructions[load2] : load2;

    return {store, val1, val2};
}

auto execute(
    std::vector<int64_t>& instructions,
    int64_t input = 0
    ) -> std::optional<int64_t>
{
    std::optional<int64_t> output{};

    int64_t inst_ptr = 0;

    while (true)
    {
        std::size_t param_idx = 0;

        const auto i = instructions[inst_ptr];

        auto op_code = OpCodeEnum::END;

        const auto str = std::to_string(i);

        std::cout << str << std::endl;
        std::string_view op_code_str;
        if(str.length() >= 2)
        {
            op_code_str = std::string_view{str.data() + str.length() - 2};
        }
        else
        {
            op_code_str = str;
        }

        std::cout << op_code_str << std::endl;

        if(op_code_str == "01" || op_code_str == "1")
        {
            op_code = OpCodeEnum::ADD;
            std::cout << "ADD" << std::endl;
        }
        else if(op_code_str == "02" || op_code_str == "2")
        {
            op_code = OpCodeEnum::MUL;
            std::cout << "MUL" << std::endl;
        }
        else if(op_code_str == "03" || op_code_str == "3")
        {
            op_code = OpCodeEnum::INPUT;
            std::cout << "INPUT" << std::endl;
        }
        else if(op_code_str == "04" || op_code_str == "4")
        {
            op_code = OpCodeEnum::OUTPUT;
            std::cout << "OUTPUT" << std::endl;
        }
        else if(op_code_str == "99")
        {
            op_code = OpCodeEnum::END;
            std::cout << "END" << std::endl;
        }
        else
        {
            std::cerr << "Unknown opcode: " << op_code_str << " exiting.";
            return {};
        }

        param_idx += op_code_str.length() + 1;

        auto p1_mode = ParameterModeEnum::POSITION;
        auto p2_mode = ParameterModeEnum::POSITION;

        if(op_code_str.length() - param_idx >= 0)
        {
            if(op_code_str[op_code_str.length() - param_idx] == '1')
            {
                p1_mode = ParameterModeEnum::IMMEDIATE;
            }
        }

        ++param_idx;

        if(op_code_str.length() - param_idx >= 0)
        {
            if(op_code_str[op_code_str.length() - param_idx] == '1')
            {
                p2_mode = ParameterModeEnum::IMMEDIATE;
            }
        }

        switch(op_code)
        {
        case OpCodeEnum::ADD:
        {
            const auto& [store, val1, val2] = four_inst(instructions, inst_ptr, p1_mode, p2_mode);

            std::cout << val1 << " + " << val2 << " at location " << store << std::endl;

            instructions[store] = val1 + val2;
            inst_ptr += 4;
        }
        break;
        case OpCodeEnum::MUL:
        {
            const auto& [store, val1, val2] = four_inst(instructions, inst_ptr, p1_mode, p2_mode);

            std::cout << val1 << " * " << val2 << " at location " << store << std::endl;

            instructions[store] = val1 * val2;
            inst_ptr += 4;
        }
        break;
        case OpCodeEnum::INPUT:
        {
            auto store = instructions[inst_ptr + 1];
            instructions[store] = input;
            inst_ptr += 2;
        }
        break;
        case OpCodeEnum::OUTPUT:
        {
            auto load = instructions[inst_ptr + 1];
            output = (p1_mode == ParameterModeEnum::POSITION) ? instructions[load] : load;
            inst_ptr += 2;

            if(output.value() != 0)
            {
                std::cerr << "Output diagnostic test failed with value: " << output.value() << std::endl;
            }
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
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    auto instructions = read_instructions(args[1]);

    auto output = execute(instructions, 1);
    if(output.has_value())
    {
        std::cout << output.value() << std::endl;
    }
    else
    {
        std::cout << "Program had no output." << std::endl;
    }

    return 0;
}