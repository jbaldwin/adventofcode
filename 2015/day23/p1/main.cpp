#include <iostream>
#include <vector>
#include <string>

#include <lib/FileUtil.h>
#include <chain/Chain.hpp>

struct Computer
{
    uint64_t a{0};
    uint64_t b{0};

    int64_t inst_ptr{0};
    std::vector<std::string> instructions;

    auto Execute() -> void
    {
        while(true)
        {
            // Program stops when an instruction outside inst.size() is used.
            if(inst_ptr >= instructions.size() || inst_ptr < 0)
            {
                return;
            }

            const auto& inst = instructions[inst_ptr];

            auto parts = chain::str::split(inst, ' ');

            if(parts[0] == "hlf")
            {
                if(parts[1] == "a")
                {
                    a /= 2;
                }
                else
                {
                    b /= 2;
                }

                ++inst_ptr;
            }
            else if(parts[0] == "tpl")
            {
                if(parts[1] == "a")
                {
                    a *= 3;
                }
                else
                {
                    b *= 3;
                }

                ++inst_ptr;
            }
            else if(parts[0] == "inc")
            {
                if(parts[1] == "a")
                {
                    ++a;
                }
                else
                {
                    ++b;
                }

                ++inst_ptr;
            }
            else if(parts[0] == "jmp")
            {
                inst_ptr += std::stol(std::string{parts[1]});
            }
            else if(parts[0] == "jie")
            {
                bool jump{false};
                if(parts[1] == "a,")
                {
                    if(a % 2 == 0) jump = true;
                }
                else
                {
                    if(b % 2 == 0) jump = true;
                }

                if(jump)
                {
                    inst_ptr += std::stol(std::string{parts[2]});
                }
                else
                {
                    ++inst_ptr;
                }
            }
            else if(parts[0] == "jio")
            {
                bool jump{false};
                if(parts[1] == "a,")
                {
                    if(a == 1) jump = true;
                }
                else
                {
                    if(b == 1) jump = true;
                }

                if(jump)
                {
                    inst_ptr += std::stol(std::string{parts[2]});
                }
                else
                {
                    ++inst_ptr;
                }
            }
            else
            {
                throw std::runtime_error("unknown instruction " + std::string{parts[0]});
            }
        }
    }
};

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);
    auto lines = chain::str::split(contents, '\n');

    Computer comp1{};
    for(const auto& line : lines)
    {
        comp1.instructions.push_back(std::string{line});
    }
    Computer comp2 = comp1;

    comp1.Execute();
    std::cout << "Part 1\n";
    std::cout << "a=" << comp1.a << "\n";
    std::cout << "b=" << comp1.b << "\n";

    comp2.a = 1;
    comp2.Execute();
    std::cout << "Part 2\n";
    std::cout << "a=" << comp2.a << "\n";
    std::cout << "b=" << comp2.b << "\n";

    return 0;
}
