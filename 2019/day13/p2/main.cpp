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

using Screen = std::map<int64_t, std::map<int64_t, TileIdEnum>>;

auto print_screen(Screen& screen) -> void
{
    int64_t x_max = screen.end()->first;

    int64_t y_max{0};
    for(const auto& [x_i, row]: screen)
    {
        y_max = std::max(y_max, row.end()->first);
    }

    for(size_t x = 0; x < x_max; ++x)
    {
        for(size_t y = 0; y < x_max; ++y)
        {
            switch(screen[x][y])
            {
                case TileIdEnum::EMPTY:
                    std::cout << ' ';
                break;
                case TileIdEnum::WALL:
                    std::cout << '#';
                break;
                case TileIdEnum::BLOCK:
                    std::cout << '+';
                break;
                case TileIdEnum::HORIZONTAL_PADDLE:
                    std::cout << '-';
                break;
                case TileIdEnum::BALL:
                    std::cout << 'O';
                break;
            }
        }
        std::cout << "\n";
    }

    std::cout << "\n\n";
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <instruction_file>" << std::endl;
        return 1;
    }

    intcode::Interpreter interpreter{args[1]};

    // Insert coins to play for free.
    interpreter.Memory(0, 2);

    using ExecStatus = intcode::Interpreter::ExecuteStatusEnum;

    std::map<int64_t, std::map<int64_t, TileIdEnum>> screen{};

    int64_t score{0};

    auto count_blocks = [&]() -> int64_t
    {
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
        return blocks;
    };

    // Track the ball movements and always keep the paddle under the ball!
    int64_t ball_x{0};
    int64_t ball_y{0};

    int64_t paddle_x{0};
    int64_t paddle_y{0};

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
                print_screen(screen);

                if(ball_x == paddle_x)
                {
                    interpreter.Input(0);
                }
                else if(ball_x > paddle_x)
                {
                    interpreter.Input(1);
                }
                else //if(ball_x < paddle_x)
                {
                    interpreter.Input(-1);
                }

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

                if(x == -1 && y == 0)
                {
                    score = interpreter.Output();
                    std::cout << "Block count: " << count_blocks() << std::endl;
                    std::cout << "Score: " << score << std::endl;
                }
                else
                {
                    auto tile_id = static_cast<TileIdEnum>(interpreter.Output());
                    screen[x][y] = tile_id;

                    if(tile_id == TileIdEnum::BALL)
                    {
                        ball_x = x;
                        ball_y = y;
                    }
                    else if(tile_id == TileIdEnum::HORIZONTAL_PADDLE)
                    {
                        paddle_x = x;
                        paddle_y = y;
                    }
                }
            }
            break;
        }
    }

    return 0;
}
