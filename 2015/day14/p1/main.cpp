#include <iostream>
#include <vector>
#include <string>

#include <lib/FileUtil.h>
#include <chain/chain.hpp>

struct Reindeer
{
    std::string name{};
    int64_t fly_speed{0};
    int64_t fly_duration{0};
    int64_t rest_duration{0};

    int64_t distance_travelled{0};

    friend auto operator<<(std::ostream& os, const Reindeer& r) -> std::ostream&
    {
        os << "{";
        os << r.name << " " << r.fly_speed << " km/s for " << r.fly_duration << "s ";
        os << "Rests for " << r.rest_duration << "s ";
        os << "Distance travelled " << r.distance_travelled;
        os << "}";

        return os;
    }
};

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 3)
    {
        std::cout << args[0] << " <input_file> <race_duration_s>" << std::endl;
        return 1;
    }

    const auto race_duration_s = std::stol(args[2]);

    auto contents = file::read(args[1]);
    auto lines = chain::str::split(contents, '\n');

    std::vector<Reindeer> reindeer{};

    for(const auto& line : lines)
    {
        if(line.empty()) continue;

        auto parts = chain::str::split(line, ' ');

        Reindeer r{};

        r.name = std::string{parts[0]};
        r.fly_speed = std::stol(std::string{parts[3]});
        r.fly_duration = std::stol(std::string{parts[6]});
        r.rest_duration = std::stol(std::string{parts[13]});

        reindeer.push_back(std::move(r));
    }

    for(auto& r : reindeer)
    {
        int64_t duration = race_duration_s;

        bool moving{true};
        while(duration > 0)
        {
            if(moving)
            {
                if(duration > r.fly_duration)
                {
                    duration -= r.fly_duration;
                    r.distance_travelled += r.fly_duration * r.fly_speed;
                }
                else
                {
                    r.distance_travelled += duration * r.fly_speed;
                    duration = 0;
                }
            }
            else
            {
                duration -= r.rest_duration;
            }
    
            moving = !moving;
        }
    }

    Reindeer* max_r{nullptr};
    int64_t max_distance{0};
    for(auto& r : reindeer)
    {
        if(r.distance_travelled > max_distance)
        {
            max_distance = r.distance_travelled;
            max_r = &r;
        }
    }

    std::cout << *max_r << '\n';

    return 0;
}
