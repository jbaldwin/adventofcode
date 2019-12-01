#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[])
{
    std::ifstream input_file("input.txt");

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
