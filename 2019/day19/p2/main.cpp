#include <iostream>
#include <vector>
#include <map>

#include <2019/intcode/Interpreter.hpp>

auto ship_fits(
    std::map<int64_t, std::map<int64_t, bool>>& grid,
    int64_t x,
    int64_t y
) -> bool
{
    // A faster solution might store the number of consecutive 
    // x/y tractor beam items in a row so we only have to 
    // look at the previous item to calculate our item.
    for(std::size_t x1 = x; x1 < x + 100; ++x1)
    {
        if(grid[y][x1] == false) return false;
    }

    for(std::size_t y1 = y; y1 < y + 100; ++y1)
    {
        if(grid[y1][x] == false) return false;
    }

    return true;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    intcode::Interpreter start{args[1]};
    intcode::Interpreter interp{args[1]};

    std::map<int64_t, std::map<int64_t, bool>> grid{};

    int64_t x{0};
    int64_t y{0};

    bool done{false};
    while(true)
    {
        if(done)
        {
            // This intcode program only outputs a single x,y coord and quits.
            interp = start;
            done = false;
        }

        auto exec_status = interp.Execute();

        switch(exec_status)
        {
            case intcode::Interpreter::ExecuteStatusEnum::COMPLETED:
                done = true;
            break;
            case intcode::Interpreter::ExecuteStatusEnum::INPUT_REQUESTED:
            {
                interp.Input(x);
                auto expect_input = interp.Execute();
                if(expect_input != intcode::Interpreter::ExecuteStatusEnum::INPUT_REQUESTED)
                {
                    std::cerr << "Expected execute() to request the y coordinate.\n";
                }
                interp.Input(y);

                ++x;
                // random guess on how wide we need to go, a more generic algorithm to search
                // outwards would be cool to come up with
                if(x == 2000)
                {
                    x = 0;
                    ++y;
                }
            }
            break;
            case intcode::Interpreter::ExecuteStatusEnum::OUTPUT_READY:
                grid[y][x] = interp.Output();

                if(x - 100 >= 0 && y - 100 >= 0 && grid[y][x] == true)
                {
                    if(ship_fits(grid, x - 100, y - 100))
                    {
                        // for some reason the x is off by 1
                        std::cout << "Ship fits at [" << x - 101 << ',' << y - 100 << "]\n";
                        return 0;
                    }
                }
            break;
        }
    }

    return 0;
}
