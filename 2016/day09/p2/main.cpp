#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>

auto decompress(std::string_view data) -> size_t
{
    size_t length{0};
    size_t open_pos{std::string_view::npos};

    while((open_pos = data.find('(')) != std::string_view::npos)
    {
        // Strip any leading characters, they won't decompress any further.
        length += open_pos;
        data.remove_prefix(open_pos);

        auto close_pos = data.find(')');
        auto cmd = data.substr(1, close_pos - 1);
        auto cmd_parts = chain::str::split(cmd, 'x');
        auto cmd_length = chain::str::to_number<uint64_t>(cmd_parts[0]).value();
        auto cmd_times = chain::str::to_number<uint64_t>(cmd_parts[1]).value();

        // Strip the command characters.
        data.remove_prefix(close_pos + 1);

        auto cmd_data_one = data.substr(0, cmd_length);
        if(cmd_data_one.find('(') == std::string_view::npos)
        {
            // If this new data section does not contain sub commands then it can
            // safely be added to the total length.
            length += cmd_length * cmd_times;
        }
        else
        {
            // Since this sub-command contains commands expand it and decompress.
            std::string cmd_data{};
            cmd_data.reserve(cmd_length * cmd_times);

            for(size_t i = 0; i < cmd_times; ++i)
            {
                cmd_data.append(cmd_data_one);
            }

            length += decompress(cmd_data);
        }

        data.remove_prefix(cmd_length);
    }

    // Add any trailing non command characters.
    length += data.length();

    return length;
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

    std::cout << "\nlength=" << decompress(contents) << "\n";

    return 0;
}
