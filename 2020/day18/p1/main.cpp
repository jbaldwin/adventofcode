#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <variant>
#include <memory>

enum class operator_t
{
    addition,
    multiply
};

auto to_string(operator_t op) -> char
{
    switch(op)
    {
        case operator_t::addition: return '+';
        case operator_t::multiply: return '*';
    }
    return '?';
}

enum class bracket
{
    open,
    close
};

auto to_string(bracket b) -> char
{
    switch(b)
    {
        case bracket::open: return '(';
        case bracket::close: return ')';
    }
    return '?';
}

auto parse(std::string_view expr) -> std::deque<std::variant<operator_t, bracket, int64_t>>
{
    auto tokens = chain::str::split(expr, ' ');

    std::deque<std::variant<operator_t, bracket, int64_t>> nodes;

    for(auto& token : tokens)
    {
        while(token.starts_with("("))
        {
            token.remove_prefix(1);
            nodes.emplace_back(bracket::open);
        }

        if(std::isdigit(token[0]))
        {
            nodes.emplace_back(chain::str::to_number<int64_t>(token).value());
        }
        else
        {
            switch(token[0])
            {
                case '+':
                    nodes.emplace_back(operator_t::addition);
                    break;
                case '*':
                    nodes.emplace_back(operator_t::multiply);
                    break;
            }
        }

        while(token.ends_with(")"))
        {
            token.remove_suffix(1);
            nodes.emplace_back(bracket::close);
        }
    }

    return nodes;
}

auto eval(std::deque<std::variant<operator_t, bracket, int64_t>>& tokens) -> int64_t
{
    std::deque<std::deque<std::variant<operator_t, int64_t>>> scopes{};
    scopes.emplace_back();

    while(true)
    {
        auto& scope = scopes.back();

        // Because expressions are evaluated from left to right calculate evaluate values as they
        // are traversed immediately.
        if(scope.size() == 3)
        {
            auto lvalue = std::get<int64_t>(scope[0]);
            auto op = std::get<operator_t>(scope[1]);
            auto rvalue = std::get<int64_t>(scope[2]);

            scope.clear();

            int64_t result;
            switch(op)
            {
                case operator_t::addition:
                    result = lvalue + rvalue;
                    break;
                case operator_t::multiply:
                    result = lvalue * rvalue;
                    break;
            }

            scope.emplace_front(result);
        }
        else
        {
            auto token = tokens.front();
            tokens.pop_front();

            if(std::holds_alternative<int64_t>(token))
            {
                scope.emplace_back(std::get<int64_t>(token));
            }
            else if(std::holds_alternative<operator_t>(token))
            {
                scope.emplace_back(std::get<operator_t>(token));
            }
            else
            {
                auto b = std::get<bracket>(token);
                switch(b)
                {
                    case bracket::open:
                        scopes.emplace_back();
                        break;
                    case bracket::close:
                        // On close grab the current scope's result value and remove the scope
                        // from the stack of scopes.
                        auto value = scope.front();
                        scopes.pop_back();

                        // Push the result into the end of the parent scope.
                        scopes.back().emplace_back(value);
                        break;
                }
            }
        }

        // When there are no tokens left, and theres only 1 scope with 1 value then the expression
        // is copmleted.
        if(tokens.empty() && scopes.size() == 1 && scopes.front().size() == 1)
        {
            break;
        }
    }

    return std::get<int64_t>(scopes.front().front());
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

    int64_t sum{0};

    for(auto& expr_view : chain::str::split(contents, '\n'))
    {
        if(expr_view.starts_with('#'))
        {
            continue;
        }

        auto expr = parse(expr_view);
        auto value = eval(expr);

        std::cout << expr_view << " = " << value << "\n";

        sum += value;
    }

    std::cout << "\nsum of all expressions = " << sum << "\n";

    return 0;
}
