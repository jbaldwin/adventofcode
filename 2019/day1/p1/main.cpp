#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

int start(std::vector<std::string> args)
{
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream input_file{args[1]};

    uint64_t total_required_mass{0};
    uint64_t module_mass{0};

    while(input_file >> module_mass)
    {
        uint64_t mass = static_cast<uint64_t>(std::floor(module_mass / 3));
        mass -= 2;
        total_required_mass += mass;
    }

    std::cout << total_required_mass << std::endl;

    return 0;
}

int main(int argc, char* argv[])
{
    return start({argv, argv + argc});
}
