#include <lib/file_util.hpp>
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
    auto lines = chain::str::split(contents, '\n');

    auto earliest_ts = chain::str::to_number<uint64_t>(lines[0]).value();
    auto parts = chain::str::split(lines[1], ',');

    std::vector<uint64_t> bus_ids{};
    bus_ids.reserve(parts.size());

    for(const auto& p : parts)
    {
        if(p == "x")
        {
            continue;
        }

        bus_ids.emplace_back(chain::str::to_number<uint64_t>(p).value());
    }

    std::vector<uint64_t> diff{};

    std::cout << "earliest timestamp = " << earliest_ts << "\n";
    for(const auto& id : bus_ids)
    {
        uint64_t curr = 0;
        while(curr < earliest_ts)
        {
            curr += id;
        }

        diff.emplace_back(curr - earliest_ts);
    }

    auto result = std::min_element(diff.begin(), diff.end());
    auto idx = std::distance(diff.begin(), result);
    auto bus_id = bus_ids[idx];
    auto wait_time = *result;

    std::cout << "wait time = " << wait_time << " for bus id = " << bus_id << "\n";
    std::cout << (wait_time * bus_id) << "\n";

    return 0;
}
