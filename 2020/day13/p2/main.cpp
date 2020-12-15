#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>

struct bus
{
    uint64_t id;
    uint64_t offset_minutes;
};

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

    auto parts = chain::str::split(lines[1], ',');

    std::vector<bus> busses{};

    size_t offset_minutes{0};
    for(const auto& p : parts)
    {
        if(p == "x")
        {
            ++offset_minutes;
        }
        else
        {
            busses.emplace_back(chain::str::to_number<uint64_t>(p).value(), offset_minutes++);
        }
    }

    uint64_t lcm = 1;
    uint64_t time = 0;
    for(size_t i = 0; i < busses.size() - 1; ++i)
    {
        uint64_t bus_id = busses[i + 1].id;
        uint64_t idx = busses[i + 1].offset_minutes;
        lcm *= busses[i].id;
        while((time + idx) % bus_id != 0)
        {
            time += lcm;
        }
    }

    std::cout << time << "\n";

    return 0;
}
