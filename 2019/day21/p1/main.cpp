#include <iostream>
#include <vector>
#include <string>
#include <string_view>

#include <2019/intcode/Interpreter.hpp>

class SpringScript
{
public:
    enum class Operator
    {
        AND,
        OR,
        NOT
    };

    enum class Register
    {
        T,
        J,
        A,
        B,
        C,
        D
    };

    static auto to_string(Operator op) -> std::string_view
    {
        switch(op)
        {
            case Operator::AND: return "AND";
            case Operator::OR: return "OR";
            case Operator::NOT: return "NOT";
        }
    }

    static auto to_string(Register r) -> std::string_view
    {
        switch(r)
        {
            case Register::T: return "T";
            case Register::J: return "J";
            case Register::A: return "A";
            case Register::B: return "B";
            case Register::C: return "C";
            case Register::D: return "D";
        }
    }
    
    struct Instruction
    {
        Operator op;
        Register one;
        Register two;

        auto to_string() -> std::string
        {
            std::string out{};
            out.reserve(64);

            out += SpringScript::to_string(op);
            out += " ";
            out += SpringScript::to_string(one);
            out += " ";
            out += SpringScript::to_string(two);

            return out;
        }
    };

    auto Instructions() -> std::vector<Instruction>&
    {
        return instructions;
    }
    auto Instructions() const -> const std::vector<Instruction>&
    {
        return instructions;
    }

    auto to_string() -> std::string
    {
        std::string out{};
        out.reserve(1024);

        for(auto i : instructions)
        {
            out += i.to_string();
            out += "\n";
        }
        out += "WALK";

        return out;
    }

private:
    std::vector<Instruction> instructions{};
};

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    intcode::Interpreter interp{args[1]};

    using SSOP = SpringScript::Operator;
    using SSR = SpringScript::Register;
    using SSI = SpringScript::Instruction;

    SpringScript ss{};

    // 3 == hole && 4 == solid then jump
    ss.Instructions().push_back(SSI{SSOP::NOT, SSR::C, SSR::J});
    ss.Instructions().push_back(SSI{SSOP::AND, SSR::D, SSR::J});

    // If A is a hole we have to jump
    ss.Instructions().push_back(SSI{SSOP::NOT, SSR::A, SSR::T});
    ss.Instructions().push_back(SSI{SSOP::OR, SSR::T, SSR::J});

    auto ss_inst = ss.to_string();

    std::cout << ss_inst << "\n\n";

    bool done{false};

    std::string full_output{};
    full_output.reserve(1024*1024);

    while(!done)
    {
        using ES = intcode::Interpreter::ExecuteStatusEnum;

        auto exec_status = interp.Execute();

        switch(exec_status)
        {
            case ES::COMPLETED:
                done = true;
            break;
            case ES::INPUT_REQUESTED:
            {
                for(const auto c : ss_inst)
                {
                    interp.Input(static_cast<int64_t>(c));
                    interp.Execute();
                }

                // next Execute() will pickup \n
                interp.Input(static_cast<int64_t>('\n'));
            }
            break;
            case ES::OUTPUT_READY:
                if(full_output.find("Walking...\n\n") != std::string::npos)
                {
                    std::cout << interp.Output();
                }
                else
                {
                    std::cout << static_cast<char>(interp.Output());
                    full_output += static_cast<char>(interp.Output());
                }
            break;
        }
    }

    return 0;
}
