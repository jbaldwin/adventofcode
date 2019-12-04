#include <iostream>
#include <fstream>
#include <string>

#include "../../../lib/StringUtil.h"

int main(int argc, char* argv[])
{
    std::ifstream input_file("example.txt");
    std::string data{};

    while(std::getline(input_file, data))
    {
        std::string_view line{data};

        
    }

    return 0;
}
