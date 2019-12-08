#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

auto fuel_calc(int64_t mass) -> int64_t
{
    mass = static_cast<int64_t>(std::floor(mass / 3));
    mass -= 2;
    return mass;
}

int start(std::vector<std::string> args)
{
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream input_file{args[1]};

    int64_t total_required_mass{0};
    int64_t module_mass{0};

    while(input_file >> module_mass)
    {
        while(module_mass > 0)
        {
            module_mass = fuel_calc(module_mass);
            if(module_mass > 0)
            {
                total_required_mass += module_mass;
            }
        }
    }

    std::cout << total_required_mass << std::endl;

    return 0;
}

int main(int argc, char* argv[])
{
    return start({argv, argv + argc});
}
