#include <iostream>
#include <array>
#include <string>

int main(int argc, char* argv[])
{
    uint64_t total_valid = 0;

    for(uint64_t i = 231832; i <= 767346; ++i)
    {
        auto digits = std::to_string(i);

        bool invalid = false;
        bool has_duplicate = false;
        for(std::size_t j = 1; j < 6; ++j)
        {
            if(digits[j] < digits[j - 1])
            {
                invalid = true;
                break;
            }

            if(digits[j] == digits[j - 1])
            {
                has_duplicate = true;
            }
        }

        if(!invalid && has_duplicate)
        {
            ++total_valid;
        }
    }

    std::cout << total_valid << std::endl;

    return 0;
}