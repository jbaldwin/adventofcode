#include <iostream>
#include <vector>

#include <2019/intcode/Interpreter.hpp>

using Map = std::vector<std::vector<char>>;

auto print_map(
    const Map& map
) -> void
{
    for(const auto& row : map)
    {
        for(const auto& c : row)
        {
            std::cout << c;
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    intcode::Interpreter interpreter{args[1]};

    Map map{};

    map.push_back({});
    auto* row = &map.front();

    while(true)
    {
        auto exec_status = interpreter.Execute();
        if(exec_status == intcode::Interpreter::ExecuteStatusEnum::COMPLETED)
        {
            break;
        }

        char output = static_cast<char>(interpreter.Output());

        switch(output)
        {
            case '#':
                row->push_back('#');
            break;
            case '.':
                row->push_back('.');
            break;
            case '\n':
                map.push_back({});
                row = &map.back();
            break;
            case '>':
                row->push_back('>');
            break;
            case '<':
                row->push_back('<');
            break;
            case '^':
                row->push_back('^');
            break;
            case 'v':
                row->push_back('v');
            break;
            case 'X':
                row->push_back('X');
            break;
        }
    }

    print_map(map);

    std::vector<std::pair<int64_t, int64_t>> intersections{};

    for(std::size_t x = 0; x < map.size(); ++x)
    {
        for(std::size_t y = 0; y < map[x].size(); ++y)
        {
            if(map[x][y] == '#')
            {
                try
                {
                    if(    map.at(x + 1).at(y) == '#'
                        && map.at(x - 1).at(y) == '#'
                        && map.at(x).at(y + 1) == '#'
                        && map.at(x).at(y - 1) == '#'
                    )
                    {
                        map[x][y] = 'O';
                        intersections.emplace_back(x, y);
                    }
                }
                catch(...)
                {
                    // .. ignore at boundary
                }
            }
        }
    }

    print_map(map);

    int64_t sum{0};
    for(const auto& [x, y] : intersections)
    {
        std::cout << "Intersection [" << x << ',' << y << "]\n";

        sum += x * y;
    }

    std::cout << "Sum of alignment parameters = " << sum << std::endl;

    return 0;
}
