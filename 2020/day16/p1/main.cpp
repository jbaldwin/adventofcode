#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>

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

    std::vector<range> ranges{};
    std::vector<ticket> nearby_tickets{};
    std::vector<uint64_t> invalid_numbers{};

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

                for(const auto& range_part : range_parts)
                {
                    auto pair = chain::str::split(range_part, '-');

                    ranges.emplace_back(
                        chain::str::to_number<uint64_t>(pair[0]).value(),
                        chain::str::to_number<uint64_t>(pair[1]).value()
                    );
                }
            }
                break;
            case parsing_mode::your_ticket:
                // ignoring my ticket for this part
                break;
            case parsing_mode::nearby_tickets:
            {
                auto& ticket = nearby_tickets.emplace_back();

                chain::str::split_for_each(line, ',', [&](std::string_view data) {
                    ticket.numbers.emplace_back(chain::str::to_number<uint64_t>(data).value());
                    return true;
                });
            }
        }
    }

    for(const auto& ticket : nearby_tickets)
    {
        for(const auto& number : ticket.numbers)
        {
            size_t match{0};
            for(const auto& range : ranges)
            {
                if(range.in_range(number))
                {
                    ++match;
                }
            }

            if(match == 0)
            {
                invalid_numbers.emplace_back(number);
            }
        }
    }

    std::cout << "invalid numbers: " << chain::str::join(invalid_numbers, ", ") << "\n";

    std::cout << "ticket scanning error rate: "
              << std::accumulate(invalid_numbers.begin(), invalid_numbers.end(), 0ul) << "\n";

    return 0;
}
