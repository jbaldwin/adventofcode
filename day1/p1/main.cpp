#include <iostream>
#include <fstream>
#include <cmath>

int main(int argc, char* argv[])
{
    std::ifstream input_file{"input.txt"};

    uint64_t total_required_mass{0};

    uint64_t module_mass{0};

    while(input_file >> module_mass)
    {
        uint64_t mass = static_cast<uint64_t>(std::floor(module_mass / 3));
        mass -= 2;

        std::cout << mass << std::endl;

        total_required_mass += mass;
    }

    std::cout << total_required_mass << std::endl;


    return 0;
}
