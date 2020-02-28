#include <iostream>
#include <vector>
#include <string>

#include <lib/FileUtil.h>

#include <nlohmann/json.hpp>

auto count(
    nlohmann::json& element
) -> int64_t
{
    int64_t total{0};

    if(element.is_object())
    {
        for(auto& e : element.items())
        {
            // If this object has a value of 'red' ignore it and all children
            if(e.value().is_string())
            {
                if(e.value().get<std::string>() == "red")
                {
                    return 0;
                }
            }

            total += count(e.value());
        }
    }
    else if(element.is_array())
    {
        for(auto& e : element.items())
        {
            total += count(e.value());
        }
    }
    else if(element.is_number())
    {
        return element.get<int64_t>();
    }

    return total;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    auto json_raw = file::read(args[1]);
    auto json = nlohmann::json::parse(json_raw);

    int64_t total = count(json);

    std::cout << total << '\n';

    return 0;
}
