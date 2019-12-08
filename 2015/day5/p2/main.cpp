#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <string_view>

int start(std::vector<std::string> args)
{
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream input_file{args[1]};
    std::string line{};

    uint64_t nice_str_count = 0;

    while(std::getline(input_file, line))
    {
        bool found_pair = false;
        bool found_xyx = false;

        std::string_view data{line.data(), line.length()};

        for(std::size_t i = 0; i < line.length() - 2; ++i)
        {
            // Find two pairs of XY XY, but XXX if X = Y does not count for this rule.
            // e.g. 'aaa' does not count but 'aaaa' does!
            if(!found_pair)
            {
                auto pair = data.substr(i, 2);
                auto remaining = data.substr(i + 2);
                auto found = remaining.find(pair);
                if(found != std::string_view::npos)
                {
                    found_pair = true;
                }
            }

            // Check for XYX pattern, XXX is also acceptable in the rules for this case.
            // Since we don't care about the middle character just check inner and outer chars.
            if(!found_xyx && data[i] == data[i + 2])
            {
                found_xyx = true;
            }

            if(found_pair && found_xyx)
            {
                ++nice_str_count;
                break;
            }
        }
    }

    std::cout << nice_str_count << std::endl;

    return 0;
}

int main(int argc, char* argv[])
{
    return start({argv, argv + argc});
}
