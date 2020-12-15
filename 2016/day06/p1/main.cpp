#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <map>

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);

    auto lines = chain::str::split(contents, '\n');

    std::vector<std::map<char, size_t>> columns{};
    columns.resize(lines[0].length());

    for(const auto& line : lines)
    {
        size_t column{0};
        for(const auto& c : line)
        {
            columns[column][c]++;
            ++column;
        }
    }

    for(const auto& column : columns)
    {
        const auto& [c, amount] = *std::max_element(
            column.begin(),
            column.end(),
            [](const auto& p1, const auto& p2) -> bool
            {
                return p1.second < p2.second;
            }
        );

        std::cout << c;
    }

    std::cout << "\n";

    return 0;
}
