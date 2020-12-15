#include <lib/file_util.hpp>
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
        auto amounts = policy_parts[0];
        auto letter = policy_parts[1][0];

        auto amounts_parts = chain::str::split(amounts, '-');
        auto lower_amount = chain::str::to_number<uint64_t>(amounts_parts[0]).value();
        auto upper_amount = chain::str::to_number<uint64_t>(amounts_parts[1]).value();

        size_t found{0};
        for(const auto& c : password)
        {
            if(c == letter)
            {
                ++found;
            }
        }

        std::cout << line;
        if(found >= lower_amount && found <= upper_amount)
        {
            std::cout << " valid " << letter << " [" << found << "]\n";
            ++valid_passwords;
        }
        else
        {
            std::cout << " invalid " << letter << " [" << found << "]\n";
        }
    }

    std::cout << "There are " << valid_passwords << " valid passwords\n";

    return 0;
}
