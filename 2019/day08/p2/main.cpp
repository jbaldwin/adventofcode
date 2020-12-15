#include <iostream>
#include <string>
#include <vector>

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

    std::vector<std::vector<std::string>> layers{};

    while(data.length() >= layer_length)
    {
        auto layer_data = data.substr(0, layer_length);
        data = data.substr(layer_length);

        std::vector<std::string> row_data{};
        row_data.reserve(height);

        while(layer_data.length() >= width)
        {
            auto col_data = layer_data.substr(0, width);
            layer_data = layer_data.substr(width);

            row_data.emplace_back(std::move(col_data));
        }

        layers.push_back(std::move(row_data));
    }

    std::vector<std::string> image{};
    image.resize(height);
    for(auto& row : image)
    {
        row.resize(width);
    }

    for(std::size_t r = 0; r < height; ++r)
    {
        for(std::size_t c = 0; c < width; ++c)
        {
            image[r][c] = '2';
        }
    }

    for(const auto& layer : layers)
    {
        for(std::size_t r = 0; r < height; ++r)
        {
            for(std::size_t c = 0; c < width; ++c)
            {
                if(layer[r][c] != '2' && image[r][c] == '2')
                {
                    image[r][c] = layer[r][c];
                }
            }
        }
    }

    for(std::size_t r = 0; r < height; ++r)
    {
        for(std::size_t c = 0; c < width; ++c)
        {
            if(image[r][c] == '1')
            {
                std::cout << '1';
            }
            else
            {
                std::cout << ' ';
            }
            // std::cout << image[r][c];
        }
        std::cout << std::endl;
    }

    return 0;
}
