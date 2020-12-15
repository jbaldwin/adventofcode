#include <iostream>
#include <vector>
#include <string>

#include <lib/file_util.hpp>
#include <chain/chain.hpp>

enum class Action
{
    FLYING,
    RESTING
};

struct Reindeer
{
    std::string name{};
    int64_t fly_speed{0};
    int64_t fly_duration{0};
    int64_t rest_duration{0};

    int64_t distance_travelled{0};
    int64_t points{0};

    auto Fly(int64_t seconds) -> void
    {
        // Reset distance travelled since this currently
        // calculates from 0s => seconds time frame.
        // A better implementation might remember state per
        // second and only add fly_speed if the reindeer is moving
        distance_travelled = 0;

        bool moving{true};
        while(seconds > 0)
        {
            if(moving)
            {
                if(seconds > fly_duration)
                {
                    seconds -= fly_duration;
                    distance_travelled += fly_duration * fly_speed;
                }
                else
                {
                    distance_travelled += seconds * fly_speed;
                    seconds = 0;
                }
            }
            else
            {
                seconds -= rest_duration;
            }

            moving = !moving;
        }
    }

    friend auto operator<<(std::ostream& os, const Reindeer& r) -> std::ostream&
    {
        os << "{";
        os << r.name << " " << r.fly_speed << " km/s for " << r.fly_duration << "s ";
        os << "Rests for " << r.rest_duration << "s ";
        os << "Distance travelled " << r.distance_travelled;
        os << " Points " << r.points;
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

    for(int64_t s = 1; s <= race_duration_s; ++s)
    {
        int64_t max_distance{0};
        for(auto& r : reindeer)
        {
            r.Fly(s);
            max_distance = std::max(max_distance, r.distance_travelled);
        }

        // any reindeer at the max distance gets a point
        for(auto& r : reindeer)
        {
            if(r.distance_travelled == max_distance)
            {
                ++r.points;
            }
        }
    }

    Reindeer* max_r{nullptr};
    int64_t max_points{0};
    for(auto& r : reindeer)
    {
        if(r.points > max_points)
        {
            max_points = r.points;
            max_r = &r;
        }
    }

    std::cout << *max_r << '\n';

    return 0;
}
