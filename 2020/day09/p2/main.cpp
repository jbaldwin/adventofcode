#include <lib/FileUtil.h>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <queue>

enum class instruction_type
{
    acc,
    jmp,
    nop
};

struct instruction
{
    instruction_type type{instruction_type::nop};
    int64_t value{0};
};

auto swap(instruction& i) -> bool
{
    switch(i.type)
    {
        case instruction_type::jmp:
            i.type = instruction_type::nop;
            return true;
        case instruction_type::nop:
            i.type = instruction_type::jmp;
            return true;
        default:
            return false;
            break;
    }
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 3)
    {
        std::cout << args[0] << " <input_file> <preamble_length" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);
    auto lines = chain::str::split(contents, '\n');
    std::vector<uint64_t> values{};
    values.reserve(lines.size());
    for(const auto& line : lines)
    {
        values.emplace_back(chain::str::to_number<uint64_t>(line).value());
    }

    uint64_t preamble_length = chain::str::to_number<uint64_t>(args[2]).value();

    std::deque<uint64_t> working_set{};
    for(size_t i = 0; i < preamble_length; ++i)
    {
        working_set.emplace_back(values[i]);
    }

    size_t value_idx{0};
    uint64_t value_without_property{0};
    for(size_t i = preamble_length; i < values.size(); ++i)
    {
        const auto value = values[i];

        bool fits = [&]() {
            for(const auto& a : working_set)
            {
                for(const auto& b : working_set)
                {
                    if(a != b && a + b == value)
                    {
                        return true;
                    }
                }
            }
            return false;
        }();

        if(!fits)
        {
            std::cout << value << " does not meet the properties requirements\n";
            value_without_property = value;
            value_idx = i;
            break;
        }

        working_set.pop_front();
        working_set.push_back(value);
    }

    for(size_t start = 0; start < value_idx; ++start)
    {
        uint64_t accumulator = values[start];

        auto [found, end] = [&]() -> std::pair<bool, size_t> {
            for(size_t i = start + 1; i < value_idx; ++i)
            {
                accumulator += values[i];

                if(accumulator == value_without_property)
                {
                    return {true, i};
                }

                if(accumulator > value_without_property)
                {
                    break;
                }
            }
            return {false, 0};
        }();

        if(found)
        {
            std::cout << "Contiguous sum:\n";
            for(size_t i = start; i <= end; ++i)
            {
                std::cout << values[i] << ", ";
            }

            auto min = *std::min_element(
                values.begin() + start,
                values.begin() + end + 1);

            auto max = *std::max_element(
                values.begin() + start,
                values.begin() + end + 1);

            std::cout << "\n";
            std::cout << "min=" << min << ", max="<< max << "\n";
            std::cout << (min + max) << "\n";
            break;
        }
    }

    return 0;
}
