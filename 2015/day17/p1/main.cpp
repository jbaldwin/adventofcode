#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <set>

#include <lib/file_util.hpp>
#include <chain/chain.hpp>

auto fill(
    std::vector<int64_t>& remaining,
    std::vector<std::pair<std::size_t, int64_t>>& used,
    int64_t max,
    std::set<std::string>& dedupe
) -> int64_t
{
    int64_t current{0};
    for(const auto& [i, c] : used)
    {
        current += c;
    }

    if(current == max)
    {
        auto copy = used;

        std::sort(
            copy.begin(),
            copy.end(), 
            [](const std::pair<std::size_t, int64_t>& a, const std::pair<std::size_t, int64_t>& b) {
                if(a.second > b.second)
                {
                    return true;
                }

                if(a.second < b.second)
                {
                    return false;
                }

                return a.first < b.first;
            }
        );

        std::string dupe_key{};
        dupe_key.reserve(128);

        for(const auto& [i, c] : copy)
        {
            dupe_key.append("[");
            dupe_key.append(std::to_string(i));
            dupe_key.append(",");
            dupe_key.append(std::to_string(c));
            dupe_key.append("] ");
        }

        if(auto found = dedupe.find(dupe_key); found == dedupe.end())
        {
            dedupe.insert(dupe_key);
            std::cout << dupe_key << '\n';
            return 1;
        }
        // already found this combo
        return 0;
    }
    if(current > max)
    {
        return 0;
    }
    if(remaining.empty())
    {
        return 0;
    }

    int64_t combos{0};

    for(std::size_t i = 0; i < remaining.size(); ++i)
    {
        if(remaining[i] == -1)
        {
            continue;
        }

        auto amount = remaining[i];
        remaining[i] = -1;

        used.emplace_back(i, amount);
        combos += fill(remaining, used, max, dedupe);

        remaining[i] = used.back().second;
        used.pop_back();
    }

    return combos;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 3)
    {
        std::cout << args[0] << " <input_file> <liters>" << std::endl;
        return 1;
    }

    auto liters = std::stol(args[2]);

    auto contents = file::read(args[1]);
    auto lines = chain::str::split(contents, '\n');

    std::vector<int64_t> containers{};

    for(const auto& line : lines)
    {
        containers.push_back(std::stol(std::string{line}));
    }

    std::sort(containers.begin(), containers.end(), std::greater<int64_t>());

    std::vector<std::pair<std::size_t, int64_t>> used{};
    std::set<std::string> dedupe{};
    auto combos = fill(containers, used, liters, dedupe);

    std::cout << "Combinations = " << combos << "\n";

    return 0;
}
