#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <variant>

enum class instruction_type
{
    cpy,
    inc,
    dec,
    jnz
};

auto to_string(instruction_type type) -> std::string
{
    switch(type)
    {
        case instruction_type::cpy: return "cpy";
        case instruction_type::inc: return "inc";
        case instruction_type::dec: return "dec";
        case instruction_type::jnz: return "jnz";
    }
    throw std::runtime_error{"invalid instruction_type"};
}

enum class register_type
{
    a = 0,
    b = 1,
    c = 2,
    d = 3
};

auto to_string(register_type r) -> char
{
    switch(r)
    {
        case register_type::a: return 'a';
        case register_type::b: return 'b';
        case register_type::c: return 'c';
        case register_type::d: return 'd';
    }

    throw std::runtime_error{"invalid register_type"};
}

struct instruction
{
    instruction_type type;
    std::variant<int64_t, register_type> x{0};
    std::variant<int64_t, register_type> y{0};

    friend auto operator<<(std::ostream& os, const instruction& i) -> std::ostream&
    {
        os << to_string(i.type) << " ";
        if(std::holds_alternative<int64_t>(i.x))
        {
            os << std::get<int64_t>(i.x);
        }
        else
        {
            os << to_string(std::get<register_type>(i.x));
        }

        if(i.type == instruction_type::cpy || i.type == instruction_type::jnz)
        {
            if(std::holds_alternative<int64_t>(i.y))
            {
                os << std::get<int64_t>(i.y);
            }
            else
            {
                os << " " << to_string(std::get<register_type>(i.y));
            }
        }
        return os;
    }
};

struct interpreter
{
    std::array<int64_t, 4> registers;
    int64_t inst_ptr{0};
    std::vector<instruction> instructions{};

    auto execute() -> void
    {
        inst_ptr = 0;

        while(inst_ptr < instructions.size())
        {
            const auto& inst = instructions[inst_ptr];

            switch(inst.type)
            {
                case instruction_type::cpy:
                {
                    int64_t x_value{0};
                    if(std::holds_alternative<int64_t>(inst.x))
                    {
                        x_value = std::get<int64_t>(inst.x);
                    }
                    else
                    {
                        auto x_reg = static_cast<size_t>(std::get<register_type>(inst.x));
                        x_value = registers[x_reg];
                    }

                    auto y_reg = static_cast<size_t>(std::get<register_type>(inst.y));
                    registers[y_reg] = x_value;
                }
                    inst_ptr++;
                    break;
                case instruction_type::inc:
                {
                    auto reg = static_cast<size_t>(std::get<register_type>(inst.x));
                    registers[reg]++;
                }
                    inst_ptr++;
                    break;
                case instruction_type::dec:
                {
                    auto reg = static_cast<size_t>(std::get<register_type>(inst.x));
                    registers[reg]--;
                }
                    inst_ptr++;
                    break;
                case instruction_type::jnz:
                {
                    int64_t x_value{0};
                    if(std::holds_alternative<int64_t>(inst.x))
                    {
                        x_value = std::get<int64_t>(inst.x);
                    }
                    else
                    {
                        auto x_reg = static_cast<size_t>(std::get<register_type>(inst.x));
                        x_value = registers[x_reg];
                    }

                    if(x_value != 0)
                    {
                        inst_ptr += std::get<int64_t>(inst.y);
                    }
                    else
                    {
                        inst_ptr++;
                    }
                }
                    break;
            }
        }
    }
};

auto parse_param(std::string_view param) -> std::variant<int64_t, register_type>
{
    switch(param[0])
    {
        case 'a': return {register_type::a};
        case 'b': return {register_type::b};
        case 'c': return {register_type::c};
        case 'd': return {register_type::d};
        default:  return std::variant<int64_t, register_type>{chain::str::to_number<int64_t>(param).value()};
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
    auto instructions_parts = chain::str::split(contents, '\n');

    interpreter interp_p1{};

    for(auto& inst : instructions_parts)
    {
        instruction i{};

        if(inst.starts_with("cpy"))
        {
            inst.remove_prefix(4);
            i.type = instruction_type::cpy;
            auto params = chain::str::split(inst, ' ');
            i.x = parse_param(params[0]);
            i.y = parse_param(params[1]);
        }
        else if(inst.starts_with("inc"))
        {
            inst.remove_prefix(4);
            i.type = instruction_type::inc;
            i.x = parse_param(inst);
        }
        else if(inst.starts_with("dec"))
        {
            inst.remove_prefix(4);
            i.type = instruction_type::dec;
            i.x = parse_param(inst);
        }
        else if(inst.starts_with("jnz"))
        {
            inst.remove_prefix(4);
            i.type = instruction_type::jnz;
            auto params = chain::str::split(inst, ' ');
            i.x = parse_param(params[0]);
            i.y = parse_param(params[1]);
        }

        interp_p1.instructions.emplace_back(std::move(i));
    }

    interpreter interp_p2 = interp_p1;

    interp_p1.execute();
    std::cout << "part 1 a register = " << interp_p1.registers[0] << "\n";

    interp_p2.registers[2] = 1;
    interp_p2.execute();
    std::cout << "part 2 a register = " << interp_p2.registers[0] << "\n";

    return 0;
}
