#include <iostream>
#include <fstream>
#include <string>
#include <string_view>

#include "../../../lib/StringUtil.h"

int main(int argc, char* argv[])
{
    std::ifstream input_file("../p1/input.txt");
    std::string line{};

    uint64_t total_encoded_len = 0;
    uint64_t total_literal_len = 0;

    while(std::getline(input_file, line))
    {
        line = str::trim(line);

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
