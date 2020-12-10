#include <lib/FileUtil.h>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <array>

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

    std::vector<uint64_t> adaptors{};
    adaptors.emplace_back(0);
    for(const auto& line : lines)
    {
        adaptors.emplace_back(chain::str::to_number<uint64_t>(line).value());
    }

    std::sort(adaptors.begin(), adaptors.end());
    adaptors.emplace_back(adaptors.back() + 3);

    std::array<uint64_t, 3> counts{0, 0, 0};

    for(size_t i = 0; i < adaptors.size() - 1; ++i)
    {
        const auto& a = adaptors[i];
        const auto& b = adaptors[i + 1];

        counts[(b - a) - 1]++;
    }

    std::cout << "diff 1 = " << counts[0] << "\n";
    std::cout << "diff 2 = " << counts[1] << "\n";
    std::cout << "diff 3 = " << counts[2] << "\n";

    std::cout << "(" << counts[0] << " x " << counts[2] << ") = " << (counts[0] * counts[2]) << "\n";

    return 0;
}
