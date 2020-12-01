#include <iostream>
#include <vector>
#include <string>

#include <lib/FileUtil.h>
#include <chain/chain.hpp>

auto look_and_say(
    const std::string& input
) -> std::string
{
    std::string output{};
    output.reserve(1024 * 1024);

    std::size_t i{0};

    while(i < input.length())
    {
        std::size_t counter{1};
        char c = input[i];
        ++i;

        while(input[i] == c)
        {
            ++counter;
            ++i;
        }

        output += std::to_string(counter);
        output += c;
    }

    return output;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 3)
    {
        std::cout << args[0] << " <input_file> <iterations>" << std::endl;
        return 1;
    }

    auto input = file::read(args[1]);
    auto iterations = std::stoul(args[2]);

    for(std::size_t i = 0; i < iterations; ++i)
    {
        input = look_and_say(input);
    }

    std::cout << input << '\n';
    std::cout << input.length() << '\n';


    return 0;
}
