#include <iostream>
#include <vector>

#include <lib/FileUtil.h>

auto fft_apply_pattern(
    const std::vector<int64_t>& input,
    uint64_t pattern_idx
) -> int64_t
{
    static std::vector<int64_t> g_pattern {0, 1, 0, -1};

    std::vector<int64_t> pattern;
    for(const auto p : g_pattern)
    {
        for(std::size_t i = 0; i < pattern_idx; ++i)
        {
            pattern.push_back(p);
        }
    }

    int64_t total{0};
    std::size_t i{1}; // always skip the first pattern
    for(auto& value : input)
    {
        // std::cout << value << " * " << pattern[i % pattern.size()] << "  + ";
        total += value * pattern[i % pattern.size()];
        ++i;
    }

    return std::abs(total % 10);
}

auto fft(
    std::vector<int64_t> input,
    uint64_t phases
) -> std::vector<int64_t>
{
    std::vector<int64_t> output{};
    output.resize(input.size());

    for(uint64_t p = 1; p <= phases; ++p)
    {
        for(std::size_t i = 0; i < input.size(); ++i)
        {
            output[i] = fft_apply_pattern(input, i + 1);
            // std::cout << " =" << output[i] << "\n";
        }

        // std::cout << "After phase " << p << " = ";
        // for(const auto& val : output)
        // {
        //     std::cout << val;
        // }
        
        output.swap(input);
        // std::cout << "\n";
    }

    return input;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 3)
    {
        std::cout << args[0] << " <input_file> <phases>" << std::endl;
        return 1;
    }

    auto contents = file::read(args[1]);
    auto phases = std::stoul(args[2]);

    std::vector<int64_t> input{};
    input.reserve(contents.length());

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

        input.push_back(i);
    }

    auto output = fft(std::move(input), phases);

    for(std::size_t i = 0; i < 8; ++i)
    {
        std::cout << output[i];
    }
    std::cout << std::endl;

    return 0;
}
