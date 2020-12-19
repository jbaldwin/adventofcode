#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <deque>

struct range
{
    uint64_t low;
    uint64_t high;

    auto in_range(uint64_t value) const -> bool
    {
        return (value >= low && value <= high);
    }

    friend auto operator<<(std::ostream& os, const range& r) -> std::ostream&
    {
        os << r.low << "-" << r.high;
        return os;
    }
};

struct field
{
    std::string name;
    range one;
    range two;

    auto in_range(uint64_t value) const -> bool
    {
        return one.in_range(value) || two.in_range(value);
    }

    friend auto operator <<(std::ostream& os, const field& f) -> std::ostream&
    {
        os << f.name << ": " << f.one << " or " << f.two;
        return os;
    }
};

struct ticket
{
    std::vector<uint64_t> numbers;

    friend auto operator<<(std::ostream& os, const ticket& t) -> std::ostream&
    {
        os << chain::str::join(t.numbers, ',');
        return os;
    }
};

enum class parsing_mode
{
    ranges,
    your_ticket,
    nearby_tickets
};

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);

    auto mode = parsing_mode::ranges;

    std::vector<field> fields{};
    ticket your_ticket{};
    std::vector<ticket> nearby_tickets{};

    for(auto& line : chain::str::split(contents, '\n'))
    {
        if(line.empty())
        {
            continue;
        }

        if(chain::str::starts_with(line, "your ticket:"))
        {
            mode = parsing_mode::your_ticket;
            continue;
        }
        else if(chain::str::starts_with(line, "nearby tickets:"))
        {
            mode = parsing_mode::nearby_tickets;
            continue;
        }

        switch(mode)
        {
            case parsing_mode::ranges:
            {
                auto kv_parts = chain::str::split(line, ": ");
                auto range_parts = chain::str::split(kv_parts[1], " or ");

                auto name = std::string{kv_parts[0]};

                auto r1_parts = chain::str::split(range_parts[0], '-');
                auto r2_parts = chain::str::split(range_parts[1], '-');

                fields.emplace_back(
                    std::move(name),
                    range{
                        chain::str::to_number<uint64_t>(r1_parts[0]).value(),
                        chain::str::to_number<uint64_t>(r1_parts[1]).value()
                    },
                    range{
                        chain::str::to_number<uint64_t>(r2_parts[0]).value(),
                        chain::str::to_number<uint64_t>(r2_parts[1]).value()
                    }
                );
            }
                break;
            case parsing_mode::your_ticket:
            {
                chain::str::split_for_each(line, ',', [&](std::string_view data) {
                    your_ticket.numbers.emplace_back(chain::str::to_number<uint64_t>(data).value());
                });
            }
                break;
            case parsing_mode::nearby_tickets:
            {
                ticket t;

                chain::str::split_for_each(line, ',', [&](std::string_view data) {
                    t.numbers.emplace_back(chain::str::to_number<uint64_t>(data).value());
                });

                bool valid{true};
                for(const auto& number : t.numbers)
                {
                    size_t match{0};
                    for(const auto& field : fields)
                    {
                        if(field.one.in_range(number) || field.two.in_range(number))
                        {
                            ++match;
                        }
                    }

                    if(match == 0)
                    {
                        valid = false;
                    }
                }

                if(valid)
                {
                    nearby_tickets.emplace_back(std::move(t));
                }
            }
        }
    }

    // field.name => set of ticket column numbers that this field can fit.
    const auto& tickets = nearby_tickets;
    std::vector<std::pair<std::string, std::vector<uint64_t>>> possible{};
    for(const auto& field : fields)
    {
        std::vector<uint64_t> columns{};

        for(size_t col = 0; col < tickets.size(); ++col)
        {
            uint64_t fits{0};
            for(const auto& ticket : tickets)
            {
                auto n = ticket.numbers[col];

                if(field.in_range(n))
                {
                    ++fits;
                }
            }
            if(fits == tickets.size())
            {
                columns.emplace_back(col);
            }
        }

        possible.emplace_back(field.name, std::move(columns));
    }

    // Sort by the least matching to greatest matching.
    std::sort(possible.begin(), possible.end(), [&](auto& a, auto& b) {
        return a.second.size() < b.second.size();
    });

    // Now attempt to slot/place each field into a specific column of ticket numbers.  This is
    // done by iterating over the matches from 'best match' to 'worst match'.  E.g. fields that
    // only matched a single column are slotted first, and so on.
    std::map<std::string, uint64_t> field_to_columns{};
    std::vector<bool> used(nearby_tickets.size(), false);

    for(const auto& [name, columns] : possible)
    {
        for(const auto& col : columns)
        {
            if(!used[col])
            {
                field_to_columns[name] = col;
                used[col] = true;
            }
        }
    }

    uint64_t product{1};
    for(const auto& [name, col] : field_to_columns)
    {
        if(chain::str::starts_with(name, "departure"))
        {
            product *= your_ticket.numbers[col];
        }
    }

    std::cout << "product = " << product << "\n";

    return 0;
}
