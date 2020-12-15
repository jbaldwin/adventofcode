#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>

auto contains_aba(std::string_view data) -> std::pair<bool, std::string_view>
{
    // This could fail on AAA
    if(data[0] == data[2])
    {
        // So now make sure ABA
        if(data[0] != data[1])
        {
            return {true, std::string_view{data.data(), 3}};
        }
    }

    return {false, {}};
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

    size_t support_ssl{0};
    for(auto& line : chain::str::split(contents, '\n'))
    {
        std::cout << line << " ";

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

        std::vector<std::string_view> abas{};
        std::vector<std::string_view> babs{};

        for(auto& [ip_part, is_hypernet] : ip_parts)
        {
            while(ip_part.length() >= 3)
            {
                auto [matches, data] = contains_aba(ip_part);

                if(matches)
                {
                    if(is_hypernet)
                    {
                        babs.emplace_back(data);
                    }
                    else
                    {
                        abas.emplace_back(data);
                    }
                }

                ip_part.remove_prefix(1);
            }
        }

        std::cout << chain::str::join(abas, "|");
        std::cout << " ! ";
        std::cout << chain::str::join(babs, "|");

        bool found_match{false};
        for(const auto& aba : abas)
        {
            for(const auto& bab : babs)
            {
                if(aba[0] == bab[1] && aba[1] == bab[0])
                {
                    ++support_ssl;
                    found_match = true;
                    std::cout << " supports SSL";
                    break;
                }
            }

            if(found_match)
            {
                break;
            }
        }

        std::cout << "\n";
    }

    std::cout << "\n" << support_ssl << " IP Addresses support SSL\n";

    return 0;
}
