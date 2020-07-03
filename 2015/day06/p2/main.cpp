#include <iostream>
#include <fstream>
#include <array>
#include <string_view>
#include <string>

#include <chain/Chain.hpp>

enum class CommandEnum
{
    INVALID,
    TURN_ON,
    TURN_OFF,
    TOGGLE
};

int start(std::vector<std::string> args)
{
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream input_file{args[1]};
    std::string data{};

    /// 1000x1000 'false'/'off' starting point.
    std::array<std::array<uint64_t, 1000>, 1000> lights{};

    while (std::getline(input_file, data))
    {
        std::string_view line{data.data(), data.length()};

        auto is_turn_on_str_view = line.substr(0, 8);
        auto is_turn_off_str_view = line.substr(0, 9);
        auto is_toggle_str_view = line.substr(0, 7);

        auto command = CommandEnum::INVALID;

        // Parse the command and strip off the command from the line information.
        if (is_turn_on_str_view == "turn on ")
        {
            command = CommandEnum::TURN_ON;
            line = line.substr(8);
        }
        else if (is_turn_off_str_view == "turn off ")
        {
            command = CommandEnum::TURN_OFF;
            line = line.substr(9);
        }
        else if (is_toggle_str_view == "toggle ")
        {
            command = CommandEnum::TOGGLE;
            line = line.substr(7);
        }

        if (command == CommandEnum::INVALID)
        {
            std::cerr << "Invalid command " << line << std::endl;
            continue;
        }

        auto parts = chain::str::split(line, ' ');
        if (parts.size() != 3)
        {
            std::cerr << "Malformed ranges: " << line << std::endl;
            continue;
        }

        auto range1_views = chain::str::split(parts[0], ',');
        auto range2_views = chain::str::split(parts[2], ',');
        if (range1_views.size() != 2)
        {
            std::cerr << "Malformed range1: " << parts[0] << std::endl;
            continue;
        }
        if (range2_views.size() != 2)
        {
            std::cerr << "Malformed range2: " << parts[2] << std::endl;
            continue;
        }

        auto x1 = std::stoul(std::string{range1_views[0]});
        auto y1 = std::stoul(std::string{range1_views[1]});

        auto x2 = std::stoul(std::string{range2_views[0]});
        auto y2 = std::stoul(std::string{range2_views[1]});

        for (std::size_t x = x1; x <= x2; ++x)
        {
            for (std::size_t y = y1; y <= y2; ++y)
            {
                switch (command)
                {
                case CommandEnum::TURN_ON:
                    ++lights[x][y];
                    break;
                case CommandEnum::TURN_OFF:
                    if (lights[x][y] > 0)
                    {
                        --lights[x][y];
                    }
                    break;
                case CommandEnum::TOGGLE:
                    lights[x][y] += 2;
                    break;
                default:
                    std::cerr << "Impossibru command selection.";
                    break;
                }
            }
        }
    }

    std::size_t total_brightness = 0;
    for (std::size_t i = 0; i <= 999; ++i)
    {
        for (std::size_t j = 0; j <= 999; ++j)
        {
            if (lights[i][j])
            {
                total_brightness += lights[i][j];
            }
        }
    }

    std::cout << total_brightness << std::endl;

    return 0;
}

int main(int argc, char* argv[])
{
    return start({argv, argv + argc});
}
