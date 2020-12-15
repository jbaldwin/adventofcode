#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <map>

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);
    auto lines = chain::str::split(contents, '\n');

    enum class outputs
    {
        bot,
        output
    };

    struct instruction
    {
        outputs output;
        size_t index;
    };

    struct bot_data
    {
        bool done{false};
        std::vector<int64_t> chips{};
        instruction lower;
        instruction higher;
    };

    // Bot number to its output chip value.
    std::map<int64_t, int64_t> output_bins{};
    // Bot number to its currently held set of chips.
    std::map<int64_t, bot_data> bots{};

    for(auto& line : lines)
    {
        if(chain::str::starts_with(line, "value"))
        {
            line.remove_prefix(6);

            auto parts = chain::str::split(line, " goes to bot ");
            auto value = chain::str::to_number<int64_t>(parts[0]).value();
            auto bot   = chain::str::to_number<int64_t>(parts[1]).value();

            bots[bot].chips.emplace_back(value);
        }
        else
        {
            line.remove_prefix(4);

            auto bot = chain::str::to_number<int64_t>(line).value();
            auto& bot_data = bots[bot];

            line.remove_prefix(line.find(" gives low to ") + 14);

            if(chain::str::starts_with(line, "bot"))
            {
                line.remove_prefix(4);
                bot_data.lower.output = outputs::bot;
            }
            else // starts_with(line, "output")
            {
                line.remove_prefix(7);
                bot_data.lower.output = outputs::output;
            }
            bot_data.lower.index = chain::str::to_number<int64_t>(line).value();

            line.remove_prefix(line.find(" and high to ") + 13);

            if(chain::str::starts_with(line, "bot"))
            {
                line.remove_prefix(4);
                bot_data.higher.output = outputs::bot;
            }
            else // starts_with(line, "output")
            {
                line.remove_prefix(7);
                bot_data.higher.output = outputs::output;
            }
            bot_data.higher.index = chain::str::to_number<int64_t>(line).value();
        }
    }

    while(true)
    {
        bool done{true};

        for(auto& [bot, bot_data] : bots)
        {
            if(!bot_data.done)
            {
                done = false;
            }

            if(!bot_data.done && bot_data.chips.size() == 2)
            {
                auto lower = std::min(bot_data.chips[0], bot_data.chips[1]);
                auto higher = std::max(bot_data.chips[0], bot_data.chips[1]);

                if(lower == 17 && higher == 61)
                {
                    std::cout << "bot " << bot << " is comparing 61 and 17\n";
                }

                switch(bot_data.lower.output)
                {
                    case outputs::bot:
                        bots[bot_data.lower.index].chips.emplace_back(lower);
                        break;
                    case outputs::output:
                        output_bins[bot_data.lower.index] = lower;
                        break;
                }

                switch(bot_data.higher.output)
                {
                    case outputs::bot:
                        bots[bot_data.higher.index].chips.emplace_back(higher);
                        break;
                    case outputs::output:
                        output_bins[bot_data.higher.index] = higher;
                        break;
                }

                bot_data.done = true;
            }
        }

        if(done)
        {
            break;
        }
    }

    for(const auto& [i, v] : output_bins)
    {
        std::cout << "output[" << i << "]=" << v << "\n";
    }

    std::cout << (output_bins[0] * output_bins[1] * output_bins[2]) << "\n";

    return 0;
}
