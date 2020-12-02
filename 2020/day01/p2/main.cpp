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

    size_t size = numbers.size();
    for(size_t a = 0 ; a < size - 2; a++)
    {
        auto n1 = numbers[a];
        for(size_t b = a + 1; b < size - 1; b++)
        {
            auto n2 = numbers[b];
            for(size_t c = b + 1; c < size; c++)
            {
                auto n3 = numbers[c];
                if(n1 + n2 + n3 == 2020)
                {
                    auto product = n1 * n2 * n3;
                    std::cout << n1 << " + " << n2 << " + " << n3 << " = 2020\n";
                    std::cout << n1 << " * " << n2 << " * " << n3 << " = " << product << "\n";
                    return 0;
                }
            }
        }
    }

    return 0;
}
