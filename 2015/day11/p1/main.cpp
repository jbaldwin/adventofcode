#include <iostream>
#include <vector>
#include <string>

#include <lib/FileUtil.h>

auto password_contains_illegal_chars(
    const std::string& password
) -> bool
{
    if(password.find('i') != std::string::npos)
    {
        return true;
    }
    if(password.find('o') != std::string::npos)
    {
        return true;
    }
    if(password.find('l') != std::string::npos)
    {
        return true;
    }

    return false;
}

auto password_contains_straight(
    const std::string& password
) -> bool
{
    for(std::size_t i = 0; i < password.length() - 3; ++i)
    {
        if(     (password[i + 1] - 1) == password[i]
            &&  (password[i + 2] - 2) == password[i])
        {
            return true;
        }
    }

    return false;
}

auto password_contains_doubles(
    const std::string& password
) -> bool
{
    std::vector<std::string> matches = 
    {
        "aa",
        "bb",
        "cc",
        "dd",
        "ee",
        "ff",
        "gg",
        "hh",
        "ii",
        "jj",
        "kk",
        "ll",
        "mm",
        "nn",
        "oo",
        "pp",
        "qq",
        "rr",
        "ss",
        "tt",
        "uu",
        "vv",
        "ww",
        "xx",
        "yy",
        "zz"
    };

    auto count = 0;
    for(const auto& match : matches)
    {
        if(password.find(match) != std::string::npos)
        {
            ++count;
        }

        if(count >= 2)
        {
            return true;
        }
    }

    return false;
}

auto password_next(
    std::string& password
) -> void
{
    do
    {
        int64_t i = password.length() - 1;
        while(i >= 0)
        {
            // if every character is z, wrap around to all a's.
            if(i == 0 && password[i] == 'z')
            {
                for(auto& c : password)
                {
                    c = 'a';
                }
                continue; // safe to continue since all a's won't pass the rules
            }

            // if this new char is already z, increment it and move to next slot
            if(password[i] == 'z')
            {
                password[i] = 'a';
                --i;
                continue;
            }

            password[i]++;
            // std::cout << password << '\n';
            break;
        }
    } while(
        password_contains_illegal_chars(password)
        || !password_contains_straight(password)
        || !password_contains_doubles(password)
    );
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 3)
    {
        std::cout << args[0] << " <input_file> <next_count>" << std::endl;
        return 1;
    }


    auto password = file::read(args[1]);
    auto next_count = std::stoul(args[2]);

    for(std::size_t i = 0; i < next_count; ++i)
    {
        password_next(password);
        std::cout << password << '\n';
    }

    return 0;
}
