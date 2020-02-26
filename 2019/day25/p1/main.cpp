#include <iostream>
#include <vector>
#include <string>

#include <2019/intcode/Interpreter.hpp>

static const std::vector<std::string> movements
{
    "north",
    "south",
    "east",
    "west"
};

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    intcode::Interpreter interpreter{args[1]};

    bool done{false};
    while(!done)
    {
        auto exec_status = interpreter.Execute();

        switch(exec_status)
        {
            case intcode::Interpreter::ExecuteStatusEnum::COMPLETED:
                done = true;
            break;
            case intcode::Interpreter::ExecuteStatusEnum::INPUT_REQUESTED:
            {
                std::string command{};
                std::getline(std::cin, command);

                for(const auto c : command)
                {
                    interpreter.Input(c);
                    interpreter.Execute();
                }
                interpreter.Input('\n');
            }
            break;
            case intcode::Interpreter::ExecuteStatusEnum::OUTPUT_READY:
                std::cout << static_cast<char>(interpreter.Output());
            break;
        }
    }

    return 0;
}
