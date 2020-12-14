#include <iostream>
#include <vector>

#include <lib/FileUtil.h>
#include <chain/chain.hpp>
#include <lib/containers.hpp>

using Triangle = containers::Vec3<uint64_t>;

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(argv[1]);
    auto lines = chain::str::split(contents, '\n');

    uint64_t triangle_count{0};

    for(std::size_t line_idx = 0; line_idx < lines.size(); line_idx += 3)
    {
        std::array<Triangle, 3> triangles{};

        for(std::size_t j = 0; j < 3; ++j)
        {
            std::size_t idx{0};
            for(const auto& length : chain::str::split(lines[line_idx + j], ' '))
            {
                if(!length.empty())
                {
                    if(auto value = chain::str::to_number<uint64_t>(length); value.has_value())
                    {
                        triangles[idx][j] = value.value();
                        idx++;
                    }
                }
            }
        }

        for(const auto& t : triangles)
        {
            if(t.x + t.y > t.z)
            {
                if(t.x + t.z > t.y)
                {
                    if(t.y + t.z > t.x)
                    {
                        triangle_count++;
                        std::cout << t << '\n';
                    }
                }
            }
        }
    }

    std::cout << "Valid triganles = " << triangle_count << '\n';

    return 0;
}