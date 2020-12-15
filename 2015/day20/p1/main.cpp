#include <iostream>
#include <vector>
#include <string>

#include <lib/file_util.hpp>
#include <chain/chain.hpp>

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    const auto min_presents = std::stoul(file::read(args[1]));
    const auto max_house_num = min_presents / 10;

    std::vector<uint64_t> presents{};
    presents.resize(max_house_num);

    std::cout << "Max house #" << max_house_num << "\n";

    for(std::size_t i = 1; i <= max_house_num; ++i)
    {
        if(i % 1000 == 0)
        {
            std::cout << i << "\n";
        }

        auto present_per_house = i * 10;
        for(std::size_t elf = i; elf <= max_house_num; elf += i)
        {
            presents[elf] += present_per_house;
        }
    }

    uint64_t house{max_house_num};
    for(int64_t i = max_house_num; i > 0; --i)
    {
        if(presents[i] >= min_presents)
        {
            house = i;
        }
    }

    std::cout << "House #" << house << " has " << presents[house] << "\n";

    return 0;
}
