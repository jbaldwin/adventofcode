#include <iostream>
#include <map>
#include <string>

#include <lib/FileUtil.h>
#include <chain/Chain.hpp>

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    const auto min_presents = std::stol(file::read(args[1]));

    std::map<std::size_t, int64_t> houses{};

    std::size_t elf{1};
    std::size_t lowest_house_num{0};
    while(lowest_house_num == 0)
    {
        int64_t presents_per_house = elf * 11;
        for(std::size_t h = 1; h <= 50; ++h)
        {
            auto house = h * elf;
            houses[house] += presents_per_house;
        }

        // Its important for this problem to only check the first
        // house if it has more than min_presents for this elf
        // as any house past that house could still have elfs
        // delivering presents.
        if(houses[elf] >= min_presents)
        {
            lowest_house_num = elf;
            break;
        }

        ++elf;
    }

    std::cout << "House #" << lowest_house_num << " has " << houses[lowest_house_num] << "\n";

    return 0;
}
