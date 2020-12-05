#include <lib/FileUtil.h>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <map>

static auto valid_passport(std::map<std::string, bool>& passport) -> bool
{
    if(passport["byr"] && passport["iyr"] && passport["eyr"] &&
       passport["hgt"] && passport["hcl"] && passport["ecl"] &&
       passport["pid"])
    {
        return true;
    }
    return false;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);

    size_t valid_passports{0};

    auto current_passport = std::map<std::string, bool>{};

    for(const auto& line : chain::str::split(contents, '\n'))
    {
        if(line.empty())
        {
            if(valid_passport(current_passport))
            {
                ++valid_passports;
                std::cout << "\t\t[valid]\n";
            }
            else
            {
                std::cout << "\n";
            }
            current_passport.clear();
            continue;
        }

        auto parts = chain::str::split(line, ' ');
        for(const auto& part : parts)
        {
            auto data = chain::str::split(part, ':');
            std::string data_name = std::string{data[0]};
            std::string data_value = std::string{data[1]};
            current_passport[data_name] = true;

            std::cout << data_name << ":" << data_value << " ";
        }
        std::cout << "\n";
    }

    std::cout << "There are " << valid_passports << " valid passports.\n";

    return 0;
}
