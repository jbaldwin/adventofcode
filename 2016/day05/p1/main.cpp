#include <lib/file_util.hpp>
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

    std::string password;

    std::string five_zeros{"00000"};

    std::string output;
    output.resize(crypto::MD5_STRING_SIZE, '\0');

    size_t counter{0};
    while(password.length() < 8)
    {
        std::string data = contents;
        data += std::to_string(counter);

        crypto::md5_t md5{};
        md5.process(data.c_str(), data.length());
        md5.finish();

        md5.get_string(output.data());

        if(chain::str::starts_with(output, five_zeros))
        {
            std::cout << counter << "\n";
            std::cout << output << "\n\n";
            password += output[5];
        }

        ++counter;
    }

    std::cout << "password=" << password << "\n";

    return 0;
}
