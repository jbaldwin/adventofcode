#include <iostream>
#include <vector>

#include <2019/intcode/Interpreter.hpp>

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    intcode::Interpreter interp{args[1]};

    std::vector<std::vector<bool>> grid{};
    grid.resize(50);
    for(auto& row : grid)
    {
        row.resize(50);
    }

    int64_t x{0};
    int64_t y{0};

    bool done{false};
    while(true)
    {
        if(done)
        {
            // This intcode program only outputs a single x,y coord and quits.
            interp = intcode::Interpreter{args[1]};
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
                if(x == 50)
                {
                    x = 0;
                    ++y;
                }
            }
            break;
            case intcode::Interpreter::ExecuteStatusEnum::OUTPUT_READY:
                std::cout << "[" << x << ',' << y << "] " << interp.Output() << "\n";
                grid[y][x] = interp.Output();
            break;
        }

        if(x == 49 && y == 49)
        {
            break;
        }
    }

    uint64_t count{0};
    std::cout << "\n\n";
    for(std::size_t y = 0; y < 50; ++y)
    {
        for(std::size_t x = 0; x < 50; ++x)
        {
            if(grid[y][x] == true)
            {
                ++count;
                std::cout << '#';
            }
            else
            {
                std::cout << '.';
            }
        }
        std::cout << '\n';
    }

    std::cout << "Total count = " << count << "\n";

    return 0;
}
