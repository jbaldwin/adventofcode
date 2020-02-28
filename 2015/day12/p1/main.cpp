#include <iostream>
#include <vector>
#include <string>

#include <lib/FileUtil.h>

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    auto json = file::read(args[1]);

    int64_t total{0};

    for(std::size_t i = 0; i < json.length(); ++i)
    {
        try
        {
            total += std::stol(json.substr(i));

            while(json[i] == '-' || std::isdigit(json[i]))
            {
                ++i;
            }
        }
        catch(...) { }
    }

    std::cout << total << '\n';

    return 0;
}
