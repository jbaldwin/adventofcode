#include <iostream>
#include <unordered_map>
#include <string>
#include <string_view>
#include <variant>
#include <optional>

#include <lib/file_util.hpp>
#include <chain/chain.hpp>

using namespace std::string_literals;

const std::string COPY = "COPY"s;
const std::string AND = "AND"s;
const std::string OR  = "OR"s;
const std::string LSHIFT = "LSHIFT"s;
const std::string RSHIFT = "RSHIFT"s;
const std::string NOT = "NOT"s;

enum class Operator
{
    COPY,
    AND,
    OR,
    LSHIFT,
    RSHIFT,
    NOT,
};

auto to_string(Operator op) -> const std::string&
{
    switch(op)
    {
        case Operator::COPY: return COPY;
        case Operator::AND: return AND;
        case Operator::OR: return OR;
        case Operator::LSHIFT: return LSHIFT;
        case Operator::RSHIFT: return RSHIFT;
        case Operator::NOT: return NOT;
    }
    throw std::runtime_error("Invalid Operator 'UNKNOWN'.");
}

struct Instruction
{
    std::string m_store;
    // string is an 'id' and uint16_t is a literal
    std::variant<std::string, uint16_t> m_l_operand;
    Operator m_operator;
    // might not be used if unary operator
    std::variant<std::string, uint16_t> m_r_operand;

    friend std::ostream& operator<<(std::ostream& os, const Instruction& i);
};

std::ostream& operator<<(std::ostream& os, const Instruction& i)
{
    switch(i.m_operator)
    {
    case Operator::COPY:
        if(std::holds_alternative<std::string>(i.m_l_operand))
        {
            os << std::get<std::string>(i.m_l_operand);
        }
        else
        {
            os << std::get<uint16_t>(i.m_l_operand);
        }
        os << " -> " << i.m_store;
        break;
    case Operator::AND:
    case Operator::OR:
    case Operator::LSHIFT:
    case Operator::RSHIFT:
        if(std::holds_alternative<std::string>(i.m_l_operand))
        {
            os << std::get<std::string>(i.m_l_operand);
        }
        else
        {
            os << std::get<uint16_t>(i.m_l_operand);
        }
        os << " " << to_string(i.m_operator) << " ";
        if(std::holds_alternative<std::string>(i.m_r_operand))
        {
            os << std::get<std::string>(i.m_r_operand);
        }
        else
        {
            os << std::get<uint16_t>(i.m_r_operand);
        }
        os << " -> " << i.m_store;
        break;
    case Operator::NOT:
        os << NOT << " ";
        if(std::holds_alternative<std::string>(i.m_l_operand))
        {
            os << std::get<std::string>(i.m_l_operand);
        }
        else
        {
            os << std::get<uint16_t>(i.m_l_operand);
        }
        os << " -> " << i.m_store;
        break;
    }

    return os;
}

auto extract_operand(std::string_view value_view) -> std::variant<std::string, uint16_t>
{
    std::variant<std::string, uint16_t> out;

    auto value = std::string{value_view};
    try
    {
        auto literal = std::stoi(value);
        out = static_cast<uint16_t>(literal);
    } catch (...)
    {
        out = std::move(value);
    }

    return out;
}

auto instructions_load_from_file(std::string_view file_name) -> std::vector<Instruction>
{
    auto raw_instructions = file::read(file_name);
    auto instructions_views = chain::str::split(raw_instructions, '\n');

    std::vector<Instruction> instructions{};
    instructions.reserve(instructions_views.size());

    for(const auto& instruction_view : instructions_views)
    {
        if(instruction_view.empty() || instruction_view.find("->") == std::string_view::npos)
        {
            continue;
        }

        auto instruction_parts = chain::str::split(instruction_view, " -> ");
        if(instruction_parts.size() != 2)
        {
            std::cerr << "Malformed instruction: " << instruction_view << std::endl;
            continue;
        }
        const auto& lhs = instruction_parts[0];
        const auto& rhs = instruction_parts[1];

        Instruction i{};
        i.m_store = std::string{rhs};

        auto lhs_parts = chain::str::split(lhs, ' ');

        if(lhs_parts.size() == 1)
        {
            i.m_operator = Operator::COPY;
            i.m_l_operand = extract_operand(lhs_parts[0]);
        }
        else if(lhs_parts.size() == 2)
        {
            if(lhs_parts[0] == NOT)
            {
                i.m_operator = Operator::NOT;
                i.m_l_operand = extract_operand(lhs_parts[1]);
            }
            else
            {
                std::cerr << "Unkown unary operator: " << instruction_view << std::endl;
                continue;
            }
        }
        else if(lhs_parts.size() == 3)
        {
            i.m_l_operand = extract_operand(lhs_parts[0]);
            i.m_r_operand = extract_operand(lhs_parts[2]);

            if(lhs_parts[1] == AND) i.m_operator = Operator::AND;
            else if(lhs_parts[1] == OR) i.m_operator = Operator::OR;
            else if(lhs_parts[1] == LSHIFT) i.m_operator = Operator::LSHIFT;
            else if(lhs_parts[1] == RSHIFT) i.m_operator = Operator::RSHIFT;
            else
            {
                std::cerr << "Instruction with unknown operator: " << instruction_view << std::endl;
                continue;
            }
        }
        else
        {
            std::cerr << "Instruction with too many lhs parameters: " << instruction_view << std::endl;
            continue;
        }

        instructions.push_back(std::move(i));
    }

    return instructions;
}

auto execute_find_binary_operands(
    const Instruction& inst,
    const std::unordered_map<std::string, uint16_t>& environment) 
    -> std::optional<std::pair<uint16_t, uint16_t>>
{
    std::optional<uint16_t> left_value{};
    std::optional<uint16_t> right_value{};
    if(std::holds_alternative<std::string>(inst.m_l_operand))
    {
        const auto& id = std::get<std::string>(inst.m_l_operand);
        auto found = environment.find(id);
        if(found != environment.end())
        {
            left_value = found->second;
        }
    }
    else
    {
        left_value = std::get<uint16_t>(inst.m_l_operand);
    }

    if(std::holds_alternative<std::string>(inst.m_r_operand))
    {
        const auto& id = std::get<std::string>(inst.m_r_operand);
        auto found = environment.find(id);
        if(found != environment.end())
        {
            right_value = found->second;
        }
    }
    else
    {
        right_value = std::get<uint16_t>(inst.m_r_operand);
    }

    if(left_value.has_value() && right_value.has_value())
    {
        return {{left_value.value(), right_value.value()}};
    }
    return {};
}

auto execute(
    std::vector<Instruction> instructions,
    std::unordered_map<std::string, uint16_t> environment = {}
    ) -> std::unordered_map<std::string, uint16_t>
{
    try
    {
        // Its possible to build a dependency graph and then traverse the tree, however,
        // to build the tree in an unsorted instruction set is probably about as long as 
        // just using an O(n^2) interpretation time.  Simply loop through until we find 
        // the next instruction that has all of its dependencies met.
        while(!instructions.empty())
        {
            bool executed_instruction = false;

            for(std::size_t idx = 0; idx < instructions.size(); ++idx)
            {
                const auto& inst = instructions[idx];

                switch(inst.m_operator)
                {
                    case Operator::COPY:
                    {
                        if(std::holds_alternative<std::string>(inst.m_l_operand))
                        {
                            const auto& id = std::get<std::string>(inst.m_l_operand);
                            auto found = environment.find(id);
                            if(found != environment.end())
                            {
                                environment[inst.m_store] = found->second;
                                executed_instruction = true;
                            }
                        }
                        else
                        {
                            // Dependencies area always met for literals
                            environment[inst.m_store] = std::get<uint16_t>(inst.m_l_operand);
                            executed_instruction = true;
                        }
                    }
                        break;
                    case Operator::AND:
                    {
                        auto found = execute_find_binary_operands(inst, environment);
                        if(found.has_value())
                        {
                            auto [left, right] = found.value();
                            environment[inst.m_store] = left & right;
                            executed_instruction = true;
                        }
                    }
                        break;
                    case Operator::OR:
                    {
                        auto found = execute_find_binary_operands(inst, environment);
                        if(found.has_value())
                        {
                            auto [left, right] = found.value();
                            environment[inst.m_store] = left | right;
                            executed_instruction = true;
                        }
                    }
                        break;
                    case Operator::LSHIFT:
                    {
                        auto found = execute_find_binary_operands(inst, environment);
                        if(found.has_value())
                        {
                            auto [left, right] = found.value();
                            environment[inst.m_store] = left << right;
                            executed_instruction = true;
                        }
                    }
                        break;
                    case Operator::RSHIFT:
                    {
                        auto found = execute_find_binary_operands(inst, environment);
                        if(found.has_value())
                        {
                            auto [left, right] = found.value();
                            environment[inst.m_store] = left >> right;
                            executed_instruction = true;
                        }
                    }
                        break;
                    case Operator::NOT:
                    {
                        if(std::holds_alternative<std::string>(inst.m_l_operand))
                        {
                            const auto& id = std::get<std::string>(inst.m_l_operand);
                            auto found = environment.find(id);
                            if(found != environment.end())
                            {
                                environment[inst.m_store] = ~found->second;
                                executed_instruction = true;
                            }
                        }
                        else
                        {
                            environment[inst.m_store] = ~std::get<uint16_t>(inst.m_l_operand);
                            executed_instruction = true;
                        }
                    }
                        break;
                }

                if(executed_instruction)
                {
                    instructions.erase(instructions.begin() + idx);
                    break;
                }
            }
        }

    } 
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return environment;
}

int start(std::vector<std::string> args)
{
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    auto instructions = instructions_load_from_file(args[1]);

    /// probably easier to just edit the input file... but this was cool /shrug!
    for(auto& inst : instructions)
    {
        if(inst.m_store == "b")
        {
            inst.m_operator = Operator::COPY;
            inst.m_l_operand = static_cast<uint16_t>(956);
        }
    }

    auto env = execute(instructions);

    std::cout << "a: " << env["a"] << std::endl;

    return 0;
}

int main(int argc, char* argv[])
{
    return start({argv, argv + argc});
}
