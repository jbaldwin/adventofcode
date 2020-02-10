#include <iostream>
#include <vector>
#include <map>
#include <numeric>

#include <lib/FileUtil.h>

auto fft(
    std::vector<int64_t> input,
    uint64_t phases
) -> std::vector<int64_t>
{
    std::vector<int64_t> output{};
    output.resize(input.size());

    for(uint64_t p = 1; p <= phases; ++p)
    {
        int64_t sum = std::accumulate(input.begin(), input.end(), 0);

        for(int64_t i = 0; i < input.size(); ++i)
        {
            output[i] = ((sum % 10) + 10) % 10;
            sum -= input[i];
        }
        
        output.swap(input);
    }

    return input;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 4)
    {
        std::cout << args[0] << " <input_file> <phases> <repeat>" << std::endl;
        return 1;
    }

    auto contents = file::read(args[1]);
    auto phases = std::stoul(args[2]);
    auto repeat = std::stoul(args[3]);

    std::vector<int64_t> orig_input{};
    orig_input.reserve(contents.length());

    while(!contents.empty())
    {
        auto s = contents.substr(0, 1);
        contents = contents.substr(1);

        int64_t i{0};

        try
        {
            i = std::stol(s);
        }
        catch(...)
        {
            continue;
        }

        orig_input.push_back(i);
    }

    std::string offset_str{};
    for(std::size_t i = 0; i < 7; ++i)
    {
        offset_str += std::to_string(orig_input[i]);
    }
    uint64_t offset = std::stoul(offset_str);

    std::vector<int64_t> input_repeated{};
    input_repeated.reserve(orig_input.size() * repeat);
    for(std::size_t r = 0; r < repeat; ++r)
    {
        for(const auto i : orig_input)
        {
            input_repeated.push_back(i);
        }
    }

    std::vector<int64_t> input{};
    input.reserve(input_repeated.size() - offset);
    for(std::size_t i = offset; i < input_repeated.size(); ++i)
    {
        input.push_back(input_repeated[i]);
    }

    auto output = fft(std::move(input), phases);
    
    std::cout << "Message=";
    for(std::size_t i = 0; i < 8; ++i)
    {
        std::cout << output[i];
    }
    std::cout << std::endl;

    return 0;
}
