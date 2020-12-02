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

    size_t valid_passwords{0};

    for(const auto& line :chain::str::split(contents, '\n'))
    {
        auto policy_password = chain::str::split(line, ": ");
        auto policy = policy_password[0];
        auto password = policy_password[1];

        auto policy_parts = chain::str::split(policy, ' ');
        auto indexes = policy_parts[0];
        auto letter = policy_parts[1][0];

        auto index_parts = chain::str::split(indexes, '-');
        auto first_index = chain::str::to_number<uint64_t>(index_parts[0]).value() - 1;
        auto second_index = chain::str::to_number<uint64_t>(index_parts[1]).value() - 1;

        bool valid{false};
        if(password[first_index] == letter && password[second_index] != letter)
        {
            valid = true;
        }
        else if(password[first_index] != letter && password[second_index] == letter)
        {
            valid = true;
        }

        std::cout << line;
        if(valid)
        {
            std::cout << " valid\n";
            ++valid_passwords;
        }
        else
        {
            std::cout << " invalid\n";
        }
    }

    std::cout << "There are " << valid_passwords << " valid passwords\n";

    return 0;
}
