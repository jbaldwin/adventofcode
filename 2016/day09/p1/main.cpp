#include <lib/FileUtil.h>
#include <chain/chain.hpp>

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
    auto contents_view = std::string_view{contents};

    std::string decompressed{};

    while(!contents_view.empty())
    {
        const auto& c = contents_view.front();
        if(c == '(')
        {
            auto close_pos = contents_view.find(')');
            auto repeater = contents_view.substr(1, close_pos - 1);
            contents_view.remove_prefix(close_pos + 1);

            auto repeater_parts = chain::str::split(repeater, 'x');
            auto length = chain::str::to_number<uint64_t>(repeater_parts[0]).value();
            auto amount = chain::str::to_number<uint64_t>(repeater_parts[1]).value();

            auto to_repeat = contents_view.substr(0, length);
            for(size_t i = 0; i < amount; ++i)
            {
                decompressed.append(to_repeat);
            }
            contents_view.remove_prefix(length);
        }
        else
        {
            decompressed += c;
            contents_view.remove_prefix(1);
        }
    }

    std::cout << "decompressed=\n" << decompressed << "\n";
    std::cout << "length=" << decompressed.length() << "\n";

    return 0;
}
