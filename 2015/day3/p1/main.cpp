#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

int main(int argc, char* argv[])
{
    std::ifstream input_file("input.txt");

    std::unordered_map<int64_t, std::unordered_map<int64_t, bool>> visited{};

    uint64_t presents_received = {0};

    std::string movements{};
    std::getline(input_file, movements);

    int64_t x = {0};
    int64_t y = {0};

    for(const auto c : movements)
    {
        switch(c)
        {
            case '^':
                ++y;
            break;
            case 'v':
                --y;
            break;
            case '<':
                --x;
            break;
            case '>':
                ++x;
            break;
            default:
                std::cerr << "Invalid movment: " << c << std::endl;
                break;
        }

        auto val = visited[x][y];
        if(!val)
        {
            ++presents_received;
            visited[x][y] = true;
        }
    }

    std::cout << presents_received << std::endl;

    return 0;
}
