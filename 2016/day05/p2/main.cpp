#include <lib/FileUtil.h>
#include <chain/chain.hpp>
#include <lib/Crypto.h>

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

    size_t found{0};
    std::string password{"FFFFFFFF"};

    std::string five_zeros{"00000"};

    std::string output;
    output.resize(crypto::MD5_STRING_SIZE, '\0');

    size_t counter{0};
    while(found < 8)
    {
        std::string data = contents;
        data += std::to_string(counter);

        crypto::md5_t md5{};
        md5.process(data.c_str(), data.length());
        md5.finish();

        md5.get_string(output.data());

        if(chain::str::starts_with(output, five_zeros))
        {
            // Ignore invalid positions.
            if(output[5] >= '0' && output[5] <= '7')
            {
                size_t position = static_cast<size_t>(output[5] - '0');

                // Only take the first solution for this position.
                if(password[position] == 'F')
                {
                    password[position] = output[6];
                    ++found;

                    std::cout << "password=" << password << "\n";
                }
            }
        }

        ++counter;
    }

    std::cout << "\npassword=" << password << "\n";

    return 0;
}
