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

static auto try_match(
    const std::map<rule_id, rule>& rules,
    std::string_view msg,
    const rule& r) -> std::vector<std::string_view>;

static auto try_match_from_sub_rule(
    const std::map<rule_id, rule>& rules,
    std::string_view msg,
    const rule_set& rs) -> std::vector<std::string_view>
{
    std::vector<std::string_view> current_matches;
    current_matches.emplace_back(msg);

    std::vector<std::string_view> next_matches;

    for(const auto& r_id : rs.rules)
    {
        // If the previous rule in this rule set produced zero matches then this is a dead end,
        // return zero matches up to the parent rule.
        if(current_matches.empty())
        {
            return {};
        }

        const auto& r = rules.find(r_id)->second;

        // Iterate through the current set of matches produced by the previous rule in this rule
        // set and attempt to apply it against the next rule in the rule set.  Store each matching
        // result for the next rule to apply to.
        for(const auto& cmatch : current_matches)
        {
            auto matches = try_match(rules, cmatch, r);
            for(const auto& m : matches)
            {
                next_matches.emplace_back(m);
            }
        }

        current_matches.swap(next_matches);
        next_matches.clear();
    }

    return current_matches;
}

static auto try_match(
    const std::map<rule_id, rule>& rules,
    std::string_view msg,
    const rule& r) -> std::vector<std::string_view>
{
    if(std::holds_alternative<std::vector<rule_set>>(r.sub_rules))
    {
        const auto& sub_rules = std::get<std::vector<rule_set>>(r.sub_rules);

        // The idea here is that there could be multiple pathways that could match with various
        // different prefix amounts removed from the input message.  Each one of these needs to
        // be explored with a self referential rule.  E.g. a self referential rule could continously
        // apply until the end of the input message string, producing many different patheways for
        // future rules to apply to at each step that it consumes.
        std::vector<std::string_view> matches;

        for(const auto& sub_rule_set : sub_rules)
        {
            auto matched = try_match_from_sub_rule(rules, msg, sub_rule_set);
            for(const auto& m : matched)
            {
                matches.emplace_back(m);
            }
        }

        return matches;
    }
    else
    {
        auto c = std::get<char>(r.sub_rules);
        if(c == msg[0])
        {
            return {msg.substr(1)};
        }
        else
        {
            return {};
        }
    }
}

static auto match(const std::map<rule_id, rule>& rules, std::string_view msg) -> bool
{
    const auto& rule_zero = rules.find(0)->second;
    auto matches = try_match(rules, msg, rule_zero);

    for(const auto& match : matches)
    {
        // An empty 'match' signifies the entire data set was consumed.
        if(match.empty())
        {
            return true;
        }
    }
    return false;
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
                if(line.find("#") != std::string_view::npos)
                {
                    continue;
                }

                messages.emplace_back(false, std::string{line});
            }
                break;
        }
    }

    uint64_t matching_msgs{0};
    for(auto& msg : messages)
    {
        if(match(rules, msg.data))
        {
            msg.matches = true;
            ++matching_msgs;
        }

        std::cout << msg << "\n";
    }

    std::cout << matching_msgs << " completely match rule zero\n";

    return 0;
}
