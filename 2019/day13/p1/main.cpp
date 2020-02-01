#include <iostream>
#include <vector>
#include <map>

#include <2019/intcode/Interpreter.hpp>

enum class TileIdEnum
{
    EMPTY = 0,
    WALL = 1,
    BLOCK = 2,
    HORIZONTAL_PADDLE = 3,
    BALL = 4
};


int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <instruction_file>" << std::endl;
        return 1;
    }

    intcode::Interpreter interpreter{args[1]};

    using ExecStatus = intcode::Interpreter::ExecuteStatusEnum;

    std::map<int64_t, std::map<int64_t, TileIdEnum>> screen{};

    bool done{false};
    while(!done)
    {
        auto status = interpreter.Execute();

        switch(status)
        {
            case ExecStatus::COMPLETED:
                done = true;
            break;
            case ExecStatus::INPUT_REQUESTED:
                throw std::runtime_error("Input was requested, invalid state.");
            break;
            case ExecStatus::OUTPUT_READY:
            {
                int64_t x = interpreter.Output();
                auto s1 = interpreter.Execute();
                if(s1 != ExecStatus::OUTPUT_READY)
                {
                    throw std::runtime_error("Expected s1 == OUTPUT_READY");
                }
                int64_t y = interpreter.Output();
                auto s2 = interpreter.Execute();
                if(s2 != ExecStatus::OUTPUT_READY)
                {
                    throw std::runtime_error("Expected s2 == OUTPUT_READY");
                }
                auto tile_id = static_cast<TileIdEnum>(interpreter.Output());

                screen[x][y] = tile_id;
            }
            break;
        }
    }

    int64_t blocks{0};
    for(const auto& [x, row] : screen)
    {
        for(const auto& [y, tile] : row)
        {
            if(tile == TileIdEnum::BLOCK)
            {
                ++blocks;
            }
        }
    }

    std::cout << "Block count: " << blocks << std::endl;

    return 0;
}
