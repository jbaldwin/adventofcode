#include <iostream>
#include <string>

#include "../../../lib/Crypto.h"

using namespace std::string_literals;

int main(int argc, char* argv[])
{
    std::string secret_key = "iwrupvqb"s;

    uint64_t counter = 1;

    std::string output;
    output.resize(crypto::MD5_STRING_SIZE, '\0');

    while(true)
    {
        auto data = secret_key + std::to_string(counter);

        crypto::md5_t md5{};
        md5.process(data.c_str(), data.length());
        md5.finish();

        md5.get_string(output.data());

        if(output[0] == '0' && output[1] == '0' && output[2] == '0' && output[3] == '0' && output[4] == '0' && output[5] == '0')
        {
            break;
        }

        ++counter;
    }

    std::cout << output << std::endl;
    std::cout << counter << std::endl;

    return 0;
}
