#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <map>

#include <lib/FileUtil.h>

static std::atomic<uint64_t> g_running{0};

auto fft_apply_pattern(
    const std::vector<int64_t>& input,
    std::size_t pattern_idx
) -> int64_t
{
    if(pattern_idx % 100 == 0)
    {
        std::cout << "Applying pattern " << pattern_idx << std::endl;
    }
    static std::vector<int64_t> g_pattern {0, 1, 0, -1};

    thread_local std::map<uint64_t, std::vector<int64_t>> t_patterns{};

    auto& pattern = t_patterns[pattern_idx];
    if(pattern.empty())
    {
        for(const auto p : g_pattern)
        {
            for(std::size_t i = 0; i < pattern_idx; ++i)
            {
                pattern.push_back(p);
            }
        }
    }

    auto p_size = pattern.size();
    auto i_size = input.size();

    int64_t total{0};
    std::size_t i{0};
    std::size_t p{1};

    while(i < i_size)
    {
        if(i > 0)
        {
            p = 0;
        }

        while(p < p_size && i < i_size)
        {
            total += input[i++] * pattern[p++];
        }
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
        std::cout << "Executing phase " << p << std::endl;
        for(std::size_t i = 0; i < input.size(); ++i)
        {
            if(i % 100 == 0)
            {
                std::cout << "Applying pattern " << i << std::endl;
            }
            static std::vector<int64_t> g_pattern {0, 1, 0, -1};
            static std::map<uint64_t, std::vector<int64_t>> t_patterns{};

            auto& pattern = t_patterns[i + 1];
            if(pattern.empty())
            {
                for(const auto p : g_pattern)
                {
                    for(std::size_t k = 0; k < i; ++k)
                    {
                        pattern.push_back(p);
                    }
                }
            }

            while(g_running >= 16)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds{10});
            }

            g_running++;
            
            std::thread worker([i, &input, &output, &pattern]() { 
                output[i] = fft_apply_pattern(input, i + 1);
                g_running--;
            });
            worker.detach();
        }

        while(g_running > 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds{10});
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

    std::vector<int64_t> input{};
    input.reserve(orig_input.size() * repeat);
    for(std::size_t r = 0; r < repeat; ++r)
    {
        for(const auto i : orig_input)
        {
            input.push_back(i);
        }
    }

    auto output = fft(std::move(input), phases);

    std::string offset_str;
    
    std::cout << "Message Offset=";
    for(std::size_t i = 0; i < 8; ++i)
    {
        std::cout << output[i];
        offset_str += std::to_string(output[i]);
    }
    std::cout << std::endl;

    // auto offset = std::stoul(offset_str);
    // std::cout << "Message=";
    // for(std::size_t i = 0; i < 8; ++i)
    // {
    //     std::cout << output[offset + i];
    // }
    // std::cout << std::endl;

    return 0;
}
