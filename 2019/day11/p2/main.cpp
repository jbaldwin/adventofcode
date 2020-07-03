#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <optional>
#include <map>
#include <array>

#include <lib/FileUtil.h>
#include <chain/Chain.hpp>
#include <lib/Algorithms.hpp>

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

class Interpreter
{
public:
    enum class ExecuteStatusEnum
    {
        INPUT_REQUESTED,
        OUTPUT_READY,
        COMPLETED
    };

    enum class OpCodeEnum
    {
        ADD                  = 1,
        MUL                  = 2,
        INPUT                = 3,
        OUTPUT               = 4,
        JUMP_IF_TRUE         = 5,
        JUMP_IF_FALSE        = 6,
        LESS_THAN            = 7,
        EQUALS               = 8,
        RELATIVE_BASE_OFFSET = 9,
        END                  = 99
    };

    enum class ParameterModeEnum
    {
        /// This is a pointer instruction and needs to be followed to either
        /// fetch or store the required data.
        POSITION  = 0,
        /// This instruction should be interpreted as the data itself.
        IMMEDIATE = 1,
        /// The parameter behaves like POSITION but with a 'relative base'.
        RELATIVE  = 2
    };

    Interpreter(
        std::vector<int64_t> instructions
    )
    {
        for(size_t i = 0; i < instructions.size(); ++i)
        {
            m_memory[i] = instructions[i];
        }

        if(m_memory.empty())
        {
            throw std::runtime_error("Interpreter instructions cannot be empty.");
        }
    }

    /// Executes the program until either output is ready, input is requested,
    /// or the program has completed.
    ///
    /// \return The current state of the program.
    auto Execute() -> ExecuteStatusEnum
    {
        if(m_completed)
        {
            return ExecuteStatusEnum::COMPLETED;
        }

        while (true)
        {
            using Mode = ParameterModeEnum;

            const auto i = m_memory[m_inst_ptr];
            const auto i_str = std::to_string(i);

            auto op_code = parse_op_code(i_str);

            auto p1_mode = parse_parameter_mode(i_str, 1);
            auto p2_mode = parse_parameter_mode(i_str, 2);
            auto p3_mode = parse_parameter_mode(i_str, 3);

            switch(op_code)
            {
            case OpCodeEnum::ADD:
            {
                // Opcode 1 adds together numbers read from two positions and stores the result
                // in a third position.  The three integers immediately after the opcode tell
                // you these three positions - the first two indicate the positions from which
                // you should read the input values, and the third indicates the position at
                // which the output should be stored.
                auto val1  = loadParam(m_inst_ptr + 1, p1_mode);
                auto val2  = loadParam(m_inst_ptr + 2, p2_mode);
                auto store_addr = storeAddr(m_inst_ptr + 3, p3_mode);
                m_memory[store_addr] = val1 + val2;
                m_inst_ptr += 4;
            }
            break;
            case OpCodeEnum::MUL:
            {
                // Opcode 2 works exactly like opcode 1, except it mutiples the two inputs
                // instead of adding them.
                auto val1  = loadParam(m_inst_ptr + 1, p1_mode);
                auto val2  = loadParam(m_inst_ptr + 2, p2_mode);
                auto store_addr = storeAddr(m_inst_ptr + 3, p3_mode);
                m_memory[store_addr] = val1 * val2;
                m_inst_ptr += 4;
            }
            break;
            case OpCodeEnum::INPUT:
            {
                // Opcode 3 takes a single integer as input and saves it to the position
                // given by its only parameter.  For example, the instruciton 3,50 would
                // take an input value and store it at address 50.
                if(m_input.has_value())
                {
                    auto store_addr = storeAddr(m_inst_ptr + 1, p1_mode);
                    m_memory[store_addr] = m_input.value();
                    m_inst_ptr += 2;
                    m_input = std::nullopt;
                }
                else
                {
                    return ExecuteStatusEnum::INPUT_REQUESTED;
                }
            }
            break;
            case OpCodeEnum::OUTPUT:
            {
                // Opcode 4 outputs the value of its only parameter.  For example, the
                // instruction 4,50 would output the value at address 50.
                m_output = loadParam(m_inst_ptr + 1, p1_mode);
                m_inst_ptr += 2;
                return ExecuteStatusEnum::OUTPUT_READY;
            }
            break;
            case OpCodeEnum::JUMP_IF_TRUE:
            {
                // Opcode 5 is jump-if-true: If the first parameter is non-zero, it sets 
                // the instruction pointer to the value from the second parameter.
                // Otherwise, it does nothing.
                auto param1 = loadParam(m_inst_ptr + 1, p1_mode);

                if(param1 != 0)
                {
                    m_inst_ptr = loadParam(m_inst_ptr + 2, p2_mode);
                }
                else
                {
                    m_inst_ptr += 3;
                }
            }
            break;
            case OpCodeEnum::JUMP_IF_FALSE:
            {
                // Opcode 6 is jump-if-false: If the first parameter is zero, it sets the
                // instruction pointer to the value from the second parameter.  Othewrise,
                // it does nothing.
                auto param1 = loadParam(m_inst_ptr + 1, p1_mode);

                if(param1 == 0)
                {
                    m_inst_ptr = loadParam(m_inst_ptr + 2, p2_mode);
                }
                else
                {
                    m_inst_ptr += 3;
                }
            }
            break;
            case OpCodeEnum::LESS_THAN:
            {
                // Opcode 7 is less than: if the first parameter is less than the second
                // parameter, it stores 1 in the position given by the third parameter.
                // Otherwise, it stores 0.
                auto param1 = loadParam(m_inst_ptr + 1, p1_mode);
                auto param2 = loadParam(m_inst_ptr + 2, p2_mode);
                auto store_addr = storeAddr(m_inst_ptr + 3, p3_mode);
                m_memory[store_addr] = (param1 < param2); 
                m_inst_ptr += 4;
            }
            break;
            case OpCodeEnum::EQUALS:
            {
                // Opcode 8 is equals: if the first parameter is equal to the second
                // parameter, it stores 1 in the position given by the third parameter.
                // Otherwise, it stores 0.
                auto param1 = loadParam(m_inst_ptr + 1, p1_mode);
                auto param2 = loadParam(m_inst_ptr + 2, p2_mode);
                auto store_addr = storeAddr(m_inst_ptr + 3, p3_mode);
                m_memory[store_addr] = (param1 == param2);
                m_inst_ptr += 4;
            }
            break;
            case OpCodeEnum::RELATIVE_BASE_OFFSET:
            {
                auto param1 = loadParam(m_inst_ptr + 1, p1_mode);
                m_relative_base += param1;
                m_inst_ptr += 2;
            }
            break;
            case OpCodeEnum::END:
            {
                m_completed = true;
                return ExecuteStatusEnum::COMPLETED;
            }
            break;
            default:
            {
                m_completed = true;
                std::cerr << "Unknown opcode: " << i << " exiting.";
                return ExecuteStatusEnum::COMPLETED;
            }
            break;
            }
        }

        m_completed = true;
        return ExecuteStatusEnum::COMPLETED;
    }

    /// Sets the given input to be used exactly once.
    ///
    /// \param input This input is used by the interpreter once.  If the \c Execute()
    ///              function encounters another input instruction and this input
    ///              has been used, then the \c Intepreter will request more input.
    auto Input(
        int64_t input
    ) -> void
    {
        m_input = input;
    }

    /// Gets the current output value after \c Execute() returned \c OUTPUT_READY
    /// execution status.
    ///
    /// \return The current output value generated by the program, defaults to 0.
    auto Output() const -> const int64_t
    {
        return m_output;
    }

    /// \return True if this program has finished, i.e. encountered a END instruction.
    auto Completed() const -> bool
    {
        return m_completed;
    }
private:
    /// The instructions and memory space to execute for this program.
    std::map<int64_t, int64_t> m_memory{};
    /// The current instruction to execute.
    int64_t m_inst_ptr{0};
    /// The current input provided to the program.  This will only ever be used once
    /// and then the \c Interpreter will request more input.
    std::optional<int64_t> m_input{std::nullopt};
    /// The current output value of the program.  May change, so when \c OUTPUT_READY 
    /// status is returned by \c Execute() this should be retrieved.
    int64_t m_output{0};
    /// True if the program has finished, i.e. encounted an \c END instruction.
    bool m_completed{false};
    /// The relative position for RELATIVE parameters.
    int64_t m_relative_base{0};

    /// \param The instruction to parse its \ref Interpreter::OpCodeEnum from.
    static auto parse_op_code(
        const std::string& instruction
    ) -> OpCodeEnum
    {
        // If the instruction length is 1 or 2 then its only the OpCodeEnum.
        if(instruction.length() <= 2)
        {
            return static_cast<OpCodeEnum>(std::stol(instruction));
        }
        else
        {
            return static_cast<OpCodeEnum>(std::stol(instruction.substr(instruction.length() - 2)));
        }
    }

    /// \param instruction The stringified instruction string.
    /// \param param_idx The parameter index, e.g. parameter 1, 2 or 3.
    /// \return The mode the parameter should be interpreted as.
    static auto parse_parameter_mode(
        const std::string& instruction,
        std::size_t param_idx
    ) -> ParameterModeEnum
    {
        // This instruction does not have any parameter modes, use default.
        if(instruction.length() <= 2)
        {
            return ParameterModeEnum::POSITION;
        }

        ssize_t inst_idx = instruction.length() - 2 - param_idx;

        // If the parameter index is within the instructions bounds
        if(inst_idx >= 0)
        {
            if(instruction[inst_idx] == '1')
            {
                return ParameterModeEnum::IMMEDIATE;
            }
            else if(instruction[inst_idx] == '2')
            {
                return ParameterModeEnum::RELATIVE;
            }
        }

        return ParameterModeEnum::POSITION;
    }

    /// Loads a parameter at the given \c inst_ptr location. 
    /// Any load out of the normal instruction range interprets that as '0'.
    ///
    /// \param inst_ptr The instruction pointer location to load the parameter from.
    /// \param mode The mode in which to load the parameter.
    auto loadParam(
        int64_t inst_ptr,
        ParameterModeEnum mode
    ) -> int64_t
    {
        switch(mode)
        {
            case ParameterModeEnum::POSITION:
                return m_memory[m_memory[inst_ptr]];
            case ParameterModeEnum::IMMEDIATE:
                return m_memory[inst_ptr];
            case ParameterModeEnum::RELATIVE:
                return m_memory[m_memory[inst_ptr] + m_relative_base];
        }
        throw std::runtime_error("Invalid parameter mode 'UNKNOWN'.");
    }

    /// Calculates the address to store a value at given the current \c inst_ptr
    /// and the parameters mode for the store parameter.
    ///
    /// \param inst_ptr This instruction points to the store parameter.
    /// \param mode The store parameters mode.
    auto storeAddr(
        int64_t inst_ptr,
        ParameterModeEnum mode
    ) -> int64_t
    {
        switch(mode)
        {
            case ParameterModeEnum::POSITION:
                return m_memory[inst_ptr];
            case ParameterModeEnum::IMMEDIATE:
                throw std::runtime_error("Invalid store mode 'IMMEDIATE'.");
            case ParameterModeEnum::RELATIVE:
                return m_memory[inst_ptr] + m_relative_base;
        }
        throw std::runtime_error("Invalid parameter mode 'UNKNOWN'.");
    }
};

enum class PaintColorEnum
{
    BLACK = 0,
    WHITE = 1
};

enum class TurnEnum
{
    LEFT_90 = 0,
    RIGHT_90 = 1
};

enum class DirectionEnum
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <instruction_file>" << std::endl;
        return 1;
    }

    auto instructions = read_instructions(args[1]);

    Interpreter interpreter{std::move(instructions)};

    std::map<int64_t, std::map<int64_t, PaintColorEnum>> hull_grid{};
    hull_grid[0][0] = PaintColorEnum::WHITE;

    int64_t x{0};
    int64_t y{0};
    DirectionEnum facing{DirectionEnum::UP};

    bool done = false;
    while(!done)
    {
        auto status = interpreter.Execute();
        switch(status)
        {
            case Interpreter::ExecuteStatusEnum::COMPLETED:
                done = true; 
            break;
            case Interpreter::ExecuteStatusEnum::INPUT_REQUESTED:
            {
                auto color = hull_grid[x][y];
                interpreter.Input(static_cast<int64_t>(color));
            }
            break;
            case Interpreter::ExecuteStatusEnum::OUTPUT_READY:
            {
                auto color = static_cast<PaintColorEnum>(interpreter.Output());
                
                if(hull_grid[x][y] != color)
                {
                    hull_grid[x][y] = color;
                }

                auto s2 = interpreter.Execute();
                if(s2 != Interpreter::ExecuteStatusEnum::OUTPUT_READY)
                {
                    throw std::runtime_error("Expected second output for direction.");
                }

                auto turn = static_cast<TurnEnum>(interpreter.Output());
                switch(turn)
                {
                    case TurnEnum::LEFT_90:
                    {
                        switch(facing)
                        {
                            case DirectionEnum::UP:
                                facing = DirectionEnum::LEFT;
                                --x;
                            break;
                            case DirectionEnum::DOWN:
                                facing = DirectionEnum::RIGHT;
                                ++x;
                            break;
                            case DirectionEnum::LEFT:
                                facing = DirectionEnum::DOWN;
                                --y;
                            break;
                            case DirectionEnum::RIGHT:
                                facing = DirectionEnum::UP;
                                ++y;
                            break;
                        }
                    }
                    break;
                    case TurnEnum::RIGHT_90:
                    {
                        switch(facing)
                        {
                            case DirectionEnum::UP:
                                facing = DirectionEnum::RIGHT;
                                ++x;
                            break;
                            case DirectionEnum::DOWN:
                                facing = DirectionEnum::LEFT;
                                --x;
                            break;
                            case DirectionEnum::LEFT:
                                facing = DirectionEnum::UP;
                                ++y;
                            break;
                            case DirectionEnum::RIGHT:
                                facing = DirectionEnum::DOWN;
                                --y;
                            break;
                        }
                    }
                    break;
                }
            }
            break;
        }
    }

    for(const auto& [x, row] : hull_grid)
    {
        for(const auto& [y, color]: row)
        {
            if(color == PaintColorEnum::WHITE)
            {
                std::cout << "#";
            }
            else
            {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }

    return 0;
}
