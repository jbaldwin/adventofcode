#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[])
{
    std::ifstream input_file("../p1/input.txt");

    std::string stairs{};
    std::getline(input_file, stairs);

    int64_t position{0};
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

        ++position;

        if(floor == -1)
        {
            break;
        }
    }

    std::cout << position << std::endl;

    return 0;
}
