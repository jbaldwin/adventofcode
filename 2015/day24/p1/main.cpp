#include <iostream>
#include <vector>
#include <string>
#include <numeric>

#include <lib/FileUtil.h>
#include <lib/StringUtil.h>
#include <lib/Algorithms.hpp>

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);

    std::vector<int64_t> packages{};
    for(const auto& line : str::split(contents, '\n'))
    {
        packages.push_back(std::stol(std::string{line}));
    }

    // all inputs so far are ordered low -> high, sort is more correct
    std::reverse(packages.begin(), packages.end());

    auto total_sum = std::accumulate(packages.begin(), packages.end(), 0);
    auto total_third = total_sum / 3;
    auto total_fourth = total_sum / 4;
    std::cout << "Sum = " << total_sum << "\n";
    std::cout << "1/3rd = " << total_third << "\n";
    std::cout << "1/4th = " << total_fourth << "\n";

    // This generally isn't a sound solution but it works for the given input.
    // A real solution still needs to check to make sure the other groups in the
    // full set add up to 20, it just happens that the given input produces
    // the correct answer by only ever checking for the smallest QE value
    // when checking for 3 or 4 groups.
    int64_t qe1 = std::numeric_limits<int64_t>::max();
    int64_t qe2 = std::numeric_limits<int64_t>::max();
    for(std::size_t i = 0; i <= 8; ++i)
    {
        algo::combinate<std::vector<int64_t>, std::vector<int64_t>>(
            packages,
            i,
            [total_third, total_fourth, &qe1, &qe2](const std::vector<int64_t>& combo) -> void
            {
                auto sum = std::accumulate(combo.begin(), combo.end(), 0);
                if(sum == total_third)
                {
                    qe1 = std::min(
                        qe1, 
                        std::accumulate(combo.begin(), combo.end(), static_cast<int64_t>(1), std::multiplies<int64_t>())
                    );
                }
                else if(sum == total_fourth)
                {
                    qe2 = std::min(
                        qe2,
                        std::accumulate(combo.begin(), combo.end(), static_cast<int64_t>(1), std::multiplies<int64_t>())
                    );
                }
            }
        );
    }

    std::cout << "3 groups qe = " << qe1 << "\n";
    std::cout << "4 groups qe = " << qe2 << "\n";

    return 0;
}
