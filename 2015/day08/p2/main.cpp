#include <iostream>
#include <fstream>
#include <string>
#include <string_view>

#include <chain/Chain.hpp>

int start(std::vector<std::string> args)
{
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream input_file{args[1]};
    std::string line{};

    uint64_t total_encoded_len = 0;
    uint64_t total_literal_len = 0;

    while(std::getline(input_file, line))
    {
        chain::str::trim(line);

        total_literal_len += line.length();

        uint64_t encoded_len = 0;

        for(const auto c : line)
        {
            if(c == '"' || c == '\\')
            {
                encoded_len += 2;
            }
            else
            {
                ++encoded_len;
            }
        }

        encoded_len += 2; // for new wrapping quotes ""
        total_encoded_len += encoded_len;
    }

    std::cout << (total_encoded_len - total_literal_len) << std::endl;

    return 0;
}

int main(int argc, char* argv[])
{
    return start({argv, argv + argc});
}
