#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

int main(int argc, char* argv[])
{
    std::ifstream input_file("../p1/input.txt");

    std::unordered_map<int64_t, std::unordered_map<int64_t, bool>> visited{};

    uint64_t presents_received = {0};

    std::string movements{};
    std::getline(input_file, movements);

    int64_t santa_x = {0};
    int64_t santa_y = {0};

    int64_t robot_x = {0};
    int64_t robot_y = {0};

    uint64_t movement_count = {0};

    for(const auto c : movements)
    {
        int64_t x = {0};
        int64_t y = {0};

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

        if(movement_count % 2 == 0) // santa
        {
            santa_x += x;
            santa_y += y;

            x = santa_x;
            y = santa_y;
        }
        else // robot
        {
            robot_x += x;
            robot_y += y;

            x = robot_x;
            y = robot_y;
        }

        auto val = visited[x][y];
        if(!val)
        {
            ++presents_received;
            visited[x][y] = true;
        }

        ++movement_count;
    }

    std::cout << presents_received << std::endl;

    return 0;
}
