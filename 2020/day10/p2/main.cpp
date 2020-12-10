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
        while(!remaining.empty())
        {
            // For each item in remaining that can connect to the last item in set issue
            // a permutation for this combination.
            auto value = remaining.back();
            if(value - set.back() <= 3)
            {
                auto s_copy = set;
                s_copy.emplace_back(remaining.back());
                remaining.pop_back();

                count += permutate(remaining, std::move(s_copy));
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

    // Reverse the adaptors so the algorithm can pop_back() from the vector to reduce shifting
    // elements when an item is moved from the remaining vector to the set vector.
    std::reverse(adaptors.begin(), adaptors.end());

    auto permutations = permutate(adaptors, {0});
    std::cout << "Total permutations = " << permutations << "\n";

    return 0;
}
