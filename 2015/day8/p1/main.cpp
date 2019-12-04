#include <iostream>
#include <fstream>
#include <string>
#include <string_view>

#include "../../../lib/StringUtil.h"

int main(int argc, char* argv[])
{
    std::ifstream input_file("input.txt");
    std::string data{};

    uint64_t total_literal_len = 0;
    uint64_t total_code_len    = 0;

    while(std::getline(input_file, data))
    {
        std::string_view line{data};

        line = str::trim(line);

        total_literal_len += line.length();

        // Remove starting end ending "" characters
        auto code_str = line;
        code_str.remove_prefix(1);
        code_str.remove_suffix(1);

        uint64_t code_len = 0;
        for(std::size_t i = 0; i < code_str.length(); ++i)
        {
            if(code_str[i] == '\\')
            {
                if(code_str[i + 1] == '\\' || code_str[i + 1] == '"')
                {
                    ++code_len;
                    ++i;
                }
                else if(code_str[i + 1] == 'x')
                {
                    ++code_len;
                    i += 3;
                }
            }
            else
            {
                ++code_len;
            }
        }

        total_code_len += code_len;
    }

    std::cout << (total_literal_len - total_code_len) << std::endl;

    return 0;
}
