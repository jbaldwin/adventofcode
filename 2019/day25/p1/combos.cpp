#include <iostream>
#include <vector>
#include <array>
#include <string>

#include "../../../lib/Algorithms.hpp"

int main(int argc, char* argv[])
{
    constexpr std::size_t CHOOSE = 3;

    std::array<std::string, 7> data = 
    {
         "mutex",
         "ornament",
         "astrolabe",
         "sand",
         "semiconductor",
         "dehydrated-water",
         "shell"
    };
    algo::combinate<std::string, 7, CHOOSE>(
        data,
        [](const std::array<std::string, CHOOSE>& result)
        {
            for(auto c : result)
            {
                std::cout << c << ' ';
            }
            std::cout << '\n';
        }
    );

    return 0;
}
