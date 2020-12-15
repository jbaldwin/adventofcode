#include <iostream>
#include <string>
#include <vector>
#include <limits>

#include <lib/file_util.hpp>

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() < 4)
    {
        std::cerr << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::string data = file::read(args[1]);

    std::size_t width = std::stoul(args[2]);
    std::size_t height = std::stoul(args[3]);

    std::size_t layer_length = width * height;

    std::vector<std::string> layers{};
    while(data.length() >= layer_length)
    {
        auto layer_data = data.substr(0, layer_length);
        data = data.substr(layer_length);

        layers.emplace_back(std::move(layer_data));
    }
    // if(!data.empty())
    // {
    //     layers.emplace_back(std::move(data));
    // }

    std::size_t fewest_zeros = std::numeric_limits<std::size_t>::max();
    std::size_t fewest_zeros_idx = 0;
    for(std::size_t i = 0; i < layers.size(); ++i)
    {
        const auto& layer = layers[i];
        std::size_t zeros = 0;
        for(const auto i : layer)
        {
            if(i == '0')
            {
                ++zeros;
            }
        }

        std::cout << layer << " zeros: " << zeros << std::endl;

        if(zeros < fewest_zeros)
        {
            fewest_zeros = zeros;
            fewest_zeros_idx = i;
        }
    }

    std::cout << "idx: " << fewest_zeros_idx << std::endl;

    std::size_t ones = 0;
    std::size_t twos = 0;

    for(const auto i : layers[fewest_zeros_idx])
    {
        if(i == '1')
        {
            ++ones;
        }
        else if(i == '2')
        {
            ++twos;
        }
    }

    std::cout << (ones * twos) << std::endl;

    return 0;
}
