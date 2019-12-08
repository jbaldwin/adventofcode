#include <iostream>
#include <fstream>
#include <vector>
#include <string>

int start(std::vector<std::string> args)
{
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }
    std::ifstream input_file{args[1]};

    std::string stairs{};
    std::getline(input_file, stairs);

    int64_t floor{0};

    for(auto c : stairs)
    {
        switch(c)
        {
            case '(':
                ++floor;
            break;
            case ')':
                --floor;
            break;
            default:
                std::cerr << "invalid input " << c << std::endl;
                break;
        }
    }

    std::cout << floor << std::endl;

    return 0;
}

int main(int argc, char* argv[])
{
    return start({argv, argv + argc});
}
