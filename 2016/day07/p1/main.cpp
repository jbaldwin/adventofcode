#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>

auto contains_abba(std::string_view data) -> bool
{
    for(size_t i = 0; i < data.length() - 3; ++i)
    {
        // This could fail on AAAA
        if(data[i] == data[i + 3] && data[i + 1] == data[i + 2])
        {
            // So now make sure ABBA
            if(data[i] != data[i + 1])
            {
                return true;
            }
        }
    }
    return false;
}

auto strip_brackets(std::string_view data) -> std::string_view
{
    if(data[0] == '[' || data[0] == ']')
    {
        data.remove_prefix(1);
    }

    if(data[data.length() - 1] == ']' || data[data.length() - 1] == '[')
    {
        data.remove_suffix(1);
    }

    return data;
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

    size_t support_tls{0};
    for(auto& line : chain::str::split(contents, '\n'))
    {
        // The ip part view and true if its a hypernet part.
        std::vector<std::pair<std::string_view, bool>> ip_parts{};

        while(true)
        {
            auto open_pos = chain::str::find(line, "[");
            if(open_pos == std::string_view::npos)
            {
                ip_parts.emplace_back(strip_brackets(line), false);
                break;
            }

            ip_parts.emplace_back(strip_brackets(line.substr(0, open_pos)), false);
            line.remove_prefix(open_pos);

            auto close_pos = chain::str::find(line, "]");
            ip_parts.emplace_back(strip_brackets(line.substr(0, close_pos)), true);
            line.remove_prefix(close_pos);
        }

        bool hypernet_contains_abba{false};
        bool ip_contains_abba{false};
        for(const auto& [s, b] : ip_parts)
        {
            if(!b)
            {
                std::cout << s;
            }
            else
            {
                std::cout << '[' << s << "]";
            }

            if(b && contains_abba(s))
            {
                hypernet_contains_abba = true;
                break;
            }
            else if(contains_abba(s))
            {
                ip_contains_abba = true;
            }
        }

        if(!hypernet_contains_abba && ip_contains_abba)
        {
            ++support_tls;
            std::cout << " VALID";
        }
        std::cout << "\n";
    }

    std::cout << "\n" << support_tls << " IP Addresses support TLS\n";

    return 0;
}
