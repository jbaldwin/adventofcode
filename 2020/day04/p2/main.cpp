#include <lib/FileUtil.h>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <cctype>

static auto valid_passport(const std::map<std::string, std::string>& passport) -> bool
{
    auto byr_found = passport.find("byr");
    if(byr_found != passport.end())
    {
        int64_t byr = chain::str::to_number<int64_t>(byr_found->second).value_or(0);
        if(byr < 1920 || byr > 2002)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    auto iyr_found = passport.find("iyr");
    if(iyr_found != passport.end())
    {
        int64_t iyr = chain::str::to_number<int64_t>(iyr_found->second).value_or(0);
        if(iyr < 2010 || iyr > 2020)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    auto eyr_found = passport.find("eyr");
    if(eyr_found != passport.end())
    {
        int64_t eyr = chain::str::to_number<int64_t>(eyr_found->second).value_or(0);
        if(eyr < 2020 || eyr > 2030)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    auto hgt_found = passport.find("hgt");
    if(hgt_found != passport.end())
    {
        auto& hgt_str = hgt_found->second;
        int64_t hgt = chain::str::to_number<int64_t>(hgt_str).value_or(0);

        if(chain::str::ends_with(hgt_str, "cm"))
        {
            if(hgt < 150 || hgt > 193)
            {
                return false;
            }
        }
        else if(chain::str::ends_with(hgt_str, "in"))
        {
            if(hgt < 59 || hgt > 76)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    auto hcl_found = passport.find("hcl");
    if(hcl_found != passport.end())
    {
        const auto& hcl = hcl_found->second;
        if(hcl.length() != 7)
        {
            return false;
        }
        if(hcl[0] != '#')
        {
            return false;
        }
        for(size_t i = 1; i < hcl.length(); ++i)
        {
            if(!std::isalnum(hcl[i]))
            {
                return false;
            }
        }
    }
    else
    {
        return false;
    }

    auto ecl_found = passport.find("ecl");
    if(ecl_found != passport.end())
    {
        static const std::vector<std::string> eye_colors{"amb", "blu", "brn", "gry", "grn", "hzl", "oth"};

        const auto& ecl = ecl_found->second;
        auto it = std::find(eye_colors.begin(), eye_colors.end(), ecl);
        if(it == eye_colors.end())
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    auto pid_found = passport.find("pid");
    if(pid_found != passport.end())
    {
        const auto& pid = pid_found->second;
        if(pid.length() != 9)
        {
            return false;
        }
        for(const auto& c : pid)
        {
            if(!std::isdigit(c))
            {
                return false;
            }
        }
    }
    else
    {
        return false;
    }

    return true;
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

    auto current_passport = std::map<std::string, std::string>{};

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
            current_passport[data_name] = data_value;

            std::cout << data_name << ":" << data_value << " ";
        }
        std::cout << "\n";
    }

    std::cout << "There are " << valid_passports << " valid passports.\n";

    return 0;
}
