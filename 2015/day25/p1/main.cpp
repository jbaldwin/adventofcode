#include <iostream>
#include <vector>
#include <string>
#include <map>

int main(int argc, char* argv[])
{
    constexpr int64_t row = 2978;
    constexpr int64_t col = 3083;

    constexpr int64_t first_code = 20151125;
    constexpr int64_t multiply_by = 252533;
    constexpr int64_t modulo_by = 33554393;

    // Pattern for generating the grid!
    // [ 1, 1, 1]
    // [ 2, 2, 1]
    // [ 3, 1, 2]
    // [ 4, 3, 1]
    // [ 5, 2, 2]
    // [ 6, 1, 3]
    // [ 7, 4, 1]
    // [ 8, 3, 2]
    // [ 9, 2, 3]
    // [10, 1, 4]
    // [11, 5, 1]
    // [12, 4, 2]
    // [13, 3, 3]
    // [14, 2, 4]
    // [15, 1, 5]

    std::map<int64_t, std::map<int64_t, int64_t>> grid{};

    // counter is the value stored at each x,y coordinate
    int64_t counter{first_code};
    // i is the number of items in this diagonal row
    int64_t i{1};
    while(true)
    {
        // Continue populating until the row/col pair is available.
        if(auto found_row = grid.find(row); found_row != grid.end())
        {
            if(auto found_col = found_row->second.find(col); found_col != found_row->second.end())
            {
                break;
            }
        }

        int64_t x = i;
        int64_t y = 1;

        // Count x down to 1 and y up to i to populate the diagonal row.
        while(x >= 1)
        {
            grid[x][y] = counter;

            x--;
            y++;

            auto value = counter * multiply_by;
            value %= modulo_by;
            counter = value;
        }

        ++i;
    }

    // for(const auto& [x, row] : grid)
    // {
    //     for(const auto& [y, value] : row)
    //     {
    //         std::cout << value << " ";
    //     }
    //     std::cout << "\n";
    // }

    std::cout << grid[row][col] << "\n";

    return 0;
}
