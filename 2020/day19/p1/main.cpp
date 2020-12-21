#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <variant>
#include <iomanip>

using rule_id = int64_t;

struct rule_set
{
    std::vector<rule_id> rules{};

    friend auto operator<<(std::ostream& os, const rule_set& rs) -> std::ostream&
    {
        bool first{true};
        for(const auto& rule : rs.rules)
        {
            if(!first)
            {
                os << " ";
            }
            else
            {
                first = false;
            }
            os << rule;
        }
        return os;
    }
};

struct rule
{
    rule_id id{0};

    using sub_rule_var_t = std::variant<
        std::vector<rule_set>,
        char
    >;

    // A rule consists of a set of ways to match against sub-rule sets
    // or a specific character to directly match.
    sub_rule_var_t sub_rules{};

    friend auto operator<<(std::ostream& os, const rule& r) -> std::ostream&
    {
        os << r.id << ": ";
        if(std::holds_alternative<std::vector<rule_set>>(r.sub_rules))
        {
            bool first{true};
            for(const auto& sub_rule : std::get<std::vector<rule_set>>(r.sub_rules))
            {
                if(!first)
                {
                    os << " | ";
                }
                else
                {
                    first = false;
                }

                os << sub_rule;
            }
        }
        else
        {
            os << "\"" << std::get<char>(r.sub_rules) << "\"";
        }
        return os;
    }
};

struct message
{
    bool matches{false};
    std::string data{};

    friend auto operator<<(std::ostream& os, const message& msg) -> std::ostream&
    {
        os << msg.data << " matches: " << std::boolalpha << msg.matches;
        return os;
    }
};

enum class parsing_mode
{
    rules,
    messages
};

static auto try_match(const std::map<rule_id, rule>& rules, std::string_view msg, const rule& r, size_t tabs = 0) -> size_t
{
    std::string tabs_str;
    for(size_t i = 0; i < tabs; ++i)
    {
        tabs_str += "\t";
    }

    std::cout << "\n";
    std::cout << tabs_str << msg << "\n";
    std::cout << tabs_str << r << "\n";

    if(std::holds_alternative<std::vector<rule_set>>(r.sub_rules))
    {
        const auto& sub_rules = std::get<std::vector<rule_set>>(r.sub_rules);

        for(const auto& sub_rule : sub_rules)
        {
            bool full_match{true};
            size_t total_matched_amount{0};
            auto msg_copy = msg;
            for(const auto& rule_id : sub_rule.rules)
            {
                const auto& rule = rules.find(rule_id)->second;

                auto amount = try_match(rules, msg_copy, rule, tabs + 1);
                if(amount == 0)
                {
                    full_match = false;
                    std::cout << tabs_str << "failed to match rule_id " << rule_id << "\n";
                    break;
                }
                else
                {
                    std::cout << tabs_str << "matched rule_id " << rule_id << "\n";
                    msg_copy.remove_prefix(amount);
                    total_matched_amount += amount;
                }
            }

            if(full_match)
            {
                return total_matched_amount;
            }
        }

        // no match
        return 0;
    }
    else
    {
        auto c = std::get<char>(r.sub_rules);
        if(c == msg[0])
        {
            std::cout << tabs_str << c << " matches\n";
            return 1;
        }
        else
        {
            std::cout << tabs_str <<c << " does not match\n";
            return 0;
        }
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

    std::map<rule_id, rule> rules{};
    std::vector<message> messages{};

    auto mode = parsing_mode::rules;
    for(auto& line : chain::str::split(contents, '\n'))
    {
        if(line.empty())
        {
            mode = parsing_mode::messages;
            continue;
        }

        switch(mode)
        {
            case parsing_mode::rules:
            {
                auto id_matches_parts = chain::str::split(line, ": ");
                auto id = chain::str::to_number<rule_id>(id_matches_parts[0]).value();

                if(id_matches_parts[1].starts_with("\""))
                {
                    rules.emplace(id, rule{id, rule::sub_rule_var_t{id_matches_parts[1][1]}});
                }
                else
                {
                    auto matches_parts = chain::str::split(id_matches_parts[1], " | ");

                    std::vector<rule_set> sub_rules{};

                    for(auto& match_part : matches_parts)
                    {
                        auto sub_rule_parts = chain::str::split_map<rule_id>(match_part, ' ', [](std::string_view data) {
                            return chain::str::to_number<rule_id>(data).value();
                        });

                        sub_rules.emplace_back(rule_set{std::move(sub_rule_parts)});
                    }

                    rules.emplace(id, rule{id, rule::sub_rule_var_t{std::move(sub_rules)}});
                }
            }
                break;
            case parsing_mode::messages:
            {
                messages.emplace_back(false, std::string{line});
            }
                break;
        }
    }

    const auto& rule_zero = rules.find(0)->second;

    uint64_t matching_msgs{0};
    for(const auto& msg : messages)
    {
        if(try_match(rules, msg.data, rule_zero) == msg.data.length())
        {
            ++matching_msgs;
        }
    }

    std::cout << matching_msgs << " completely match rule zero\n";

    return 0;
}
