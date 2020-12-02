#include <lib/FileUtil.h>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);

    std::vector<uint64_t> numbers{};
    for(const auto& line : chain::str::split(contents, '\n'))
    {
        numbers.push_back(std::stoul(std::string{line}));
    }

    // The input could contain a number like 1010 which would fail
    // since 1010+1010 == 2020 but then give the wrong answer.

    size_t size = numbers.size();
    for(size_t a = 0 ; a < size - 1; a++)
    {
        auto n1 = numbers[a];
        for(size_t b = a + 1; b < size; b++)
        {
            auto n2 = numbers[b];
            if(n1 + n2 == 2020)
            {
                auto product = n1 * n2;
                std::cout << n1 << " + " << n2 << " = 2020\n";
                std::cout << n1 << " * " << n2 << " = " << product << "\n";
                return 0;
            }
        }
    }

    return 0;
}
