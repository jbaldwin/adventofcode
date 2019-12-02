#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[])
{
    std::ifstream input_file{"input.txt"};
    std::string line{};

    uint64_t nice_str_count = 0;

    while(std::getline(input_file, line))
    {
        bool double_found = false;
        uint64_t vowel_count = 0;
        bool naughty = false;

        for(std::size_t i = 0; i < line.length(); ++i)
        {
            const auto c = line[i];

            if(i < line.length() - 1)
            {
                const auto c1 = line[i + 1];

                if(c == 'a' && c1 == 'b')
                {
                    naughty = true;
                    break;
                }

                if(c == 'c' && c1 == 'd')
                {
                    naughty = true;
                    break;
                }

                if(c == 'p' && c1 == 'q')
                {
                    naughty = true;
                    break;
                }

                if(c == 'x' && c1 == 'y')
                {
                    naughty = true;
                    break;
                }

                // double
                if(c == c1)
                {
                    double_found = true;
                }
            }

            switch(c)
            {
                case 'a':
                case 'e':
                case 'i':
                case 'o':
                case 'u':
                    ++vowel_count;
                    break;
                default:
                    break;
            }
        }

        // If there are not enough vowels or no double found, naughty!
        if(vowel_count < 3 || !double_found)
        {
            naughty = true;
        }

        if(!naughty)
        {
            ++nice_str_count;
        }
    }

    std::cout << nice_str_count << std::endl;

    return 0;
}
