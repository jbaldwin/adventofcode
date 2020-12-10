#include <lib/FileUtil.h>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <map>

static std::map<std::string, size_t> g_cache{};

static auto permutate(std::vector<uint64_t> remaining, std::vector<uint64_t> set) -> size_t
{
    if(remaining.empty())
    {
        return 1;
    }
    else
    {
        // Store calculated values for the remaining set so when it is encountered again
        // on another permutation the result can be returned immediately.
        auto key = chain::str::join(remaining, ", ");
        auto in_cache = g_cache.find(key);
        if(in_cache != g_cache.end())
        {
            return in_cache->second;
        }

        size_t count{0};
        for(size_t i = 0; i < remaining.size(); ++i)
        {
            // Loop through remaining and any that are <= 3 away generate a new valid set
            // and valid remaining and permutate on each valid new combination.
            if(remaining[i] - set.back() <= 3)
            {
                auto s_copy = set;
                s_copy.emplace_back(remaining[i]);
                auto r_copy = remaining;
                r_copy.erase(r_copy.begin(), r_copy.begin() + i + 1);

                count += permutate(std::move(r_copy), std::move(s_copy));
            }
            else
            {
                // Input is guaranteed to be ordered so no more valid combinations can exist.
                break;
            }
        }

        g_cache[key] = count;
        return count;
    }
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
    auto lines = chain::str::split(contents, '\n');

    std::vector<uint64_t> adaptors{};
    for(const auto& line : lines)
    {
        adaptors.emplace_back(chain::str::to_number<uint64_t>(line).value());
    }

    std::sort(adaptors.begin(), adaptors.end());
    adaptors.emplace_back(adaptors.back() + 3);

    std::cout << "Total permutations = " << permutate(adaptors, {0}) << "\n";

    return 0;
}
