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
        ADD             = 1,
        MUL             = 2,
        INPUT           = 3,
        OUTPUT          = 4,
        JUMP_IF_TRUE    = 5,
        JUMP_IF_FALSE   = 6,
        LESS_THAN       = 7,
        EQUALS          = 8,
        END             = 99
    };

    enum class ParameterModeEnum
    {
        /// This is a pointer instruction and needs to be followed to either
        /// fetch or store the required data.
        POSITION    = 0,
        /// This instruction should be interpreted as the data itself.
        IMMEDIATE   = 1
    };

    Interpreter(
        std::vector<int64_t> instructions
    )
        :   m_instructions(std::move(instructions))
    {
        if(m_instructions.empty())
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

            const auto i = m_instructions[m_inst_ptr];
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
                auto val1  = loadParam(m_inst_ptr + 1, p1_mode);
                auto val2  = loadParam(m_inst_ptr + 2, p2_mode);
                store(m_inst_ptr + 3, val1 + val2);
                m_inst_ptr += 4;
            }
            break;
            case OpCodeEnum::MUL:
            {
                // Opcode 2 works exactly like opcode 1, except it mutiples the two inputs
                // instead of adding them.
                auto val1  = loadParam(m_inst_ptr + 1, p1_mode);
                auto val2  = loadParam(m_inst_ptr + 2, p2_mode);
                store(m_inst_ptr + 3, val1 * val2);
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
                    store(m_inst_ptr + 1, m_input.value());
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
                store(m_inst_ptr + 3, (param1 < param2));
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
                store(m_inst_ptr + 3, (param1 == param2));
                m_inst_ptr += 4;
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
    /// The instructions to execute for this program.
    std::vector<int64_t> m_instructions{};
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

    /// \param The instruction to parse its \ref Interpreter::OpCodeEnum from.
    static auto parse_op_code(
        const std::string& instruction
    ) -> OpCodeEnum
    {
        // If the instructino length is 1 or 2 then its only the OpCodeEnum.
        if(instruction.length() == 1 || instruction.length() == 2)
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

    /// Loads a parameter at the given \c inst_ptr location. 
    ///
    /// \param inst_ptr The instruction pointer location to load the parameter from.
    /// \param mode The mode in which to load the parameter.
    auto loadParam(
        int64_t inst_ptr,
        ParameterModeEnum mode
    ) -> int64_t
    {
        return (mode == ParameterModeEnum::POSITION)
            ? m_instructions[m_instructions[inst_ptr]]
            : m_instructions[inst_ptr];
    }

    /// Stores a given value at the location pointed to by \c inst_ptr.
    ///
    /// \param inst_ptr This instruction points to the location to store \c value.
    /// \param value The value to store.
    auto store(
        int64_t inst_ptr,
        int64_t value
    ) -> void
    {
        auto store_at = m_instructions[inst_ptr];
        m_instructions[store_at] = value;
    }
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

    std::map<std::string, int64_t> results{};

    algo::permutate<std::array<int64_t, 5>>(
        std::array<int64_t, 5>{5,6,7,8,9},
        [&results, &instructions](const std::array<int64_t, 5> p) -> void
        {
            std::string key{};
            key.reserve(5);
            for(const auto& phase_setting : p)
            {
                key += std::to_string(phase_setting);
            }

            std::array<Interpreter, 5> interpreters = {
                Interpreter{instructions},
                Interpreter{instructions},
                Interpreter{instructions},
                Interpreter{instructions},
                Interpreter{instructions}
            };

            // Execute each interpreter to get past its phase setting input request.
            for(std::size_t idx = 0; idx < 5; ++idx)
            {
                auto& i = interpreters[idx];
                i.Input(p[idx]);
                auto expect = i.Execute();
                if(expect != Interpreter::ExecuteStatusEnum::INPUT_REQUESTED)
                {
                    std::cerr << "EXPECTED INPUT REQUESTED! phase setting round" << std::endl;
                    return;
                }
            }

            auto all_halted = [&interpreters]() -> bool
            {
                return 
                       interpreters[0].Completed() 
                    && interpreters[1].Completed() 
                    && interpreters[2].Completed() 
                    && interpreters[3].Completed() 
                    && interpreters[4].Completed();
            };

            interpreters[0].Input(0); // Kick off everything with zero

            // Now loop until all interpereters are finished, passing their inputs
            // and outputs along in a chain as each one finishes.
            while(!all_halted())
            {
                for(std::size_t idx = 0; idx < 5; ++idx)
                {
                    auto& i = interpreters[idx];
                    auto status = i.Execute();

                    switch(status)
                    {
                        case Interpreter::ExecuteStatusEnum::INPUT_REQUESTED:
                            // this should never happen
                            std::cerr << "UNEPECTED INPUT REQUESTED" << std::endl;
                        break;
                        case Interpreter::ExecuteStatusEnum::OUTPUT_READY:
                        {
                            auto& next = interpreters[(idx + 1) % 5];
                            next.Input(i.Output());
                        }
                        break;
                        case Interpreter::ExecuteStatusEnum::COMPLETED:
                            // calling Execute() on a completed interperter is ok
                        break;
                    } 
                }
            }

            // Grab the final output of the last 'amp' in the problem.
            results.emplace(std::move(key), interpreters[4].Output());
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