#include <lib/FileUtil.h>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <array>

// Use for example.
// constexpr const size_t g_width{7};
// constexpr const size_t g_height{3};
constexpr const size_t g_width{50};
constexpr const size_t g_height{6};

enum class rotation
{
    row,
    column
};

auto to_string(rotation rot) -> std::string
{
    switch(rot)
    {
        case rotation::row:
            return "row";
        case rotation::column:
            return "column";
    }
    throw std::runtime_error{"invalid rotation"};
}

enum class operation
{
    rect,
    rotate
};

auto to_string(operation op) -> std::string
{
    switch(op)
    {
        case operation::rect:
            return "rect";
        case operation::rotate:
            return "rotate";
    }
    throw std::runtime_error{"invalid operation"};
}

struct instruction
{
    operation op;
    rotation rot;
    uint64_t x{0};
    uint64_t y{0};
    uint64_t idx{0};
    uint64_t amount{0};

    friend auto operator<<(std::ostream& os, const instruction& i) -> std::ostream&
    {
        os << to_string(i.op) << " ";
        switch(i.op)
        {
            case operation::rect:
                os << i.x << "x" << i.y;
                break;
            case operation::rotate:
                os << to_string(i.rot);
                switch(i.rot)
                {
                    case rotation::row:
                        os << " y=" << i.idx;
                        break;
                    case rotation::column:
                        os << " x=" << i.idx;
                        break;
                }
                os << " by " << i.amount;
                break;
        }
        return os;
    }
};

struct sequence
{
    sequence()
    {
        for(auto& row : screen)
        {
            row.fill('.');
        }
    }

    std::array<std::array<char, g_width>, g_height> screen;

    std::vector<instruction> instructions{};

    auto advance() -> bool
    {
        if(inst_ptr < instructions.size())
        {
            const auto& inst = instructions[inst_ptr];
            std::cout << inst << "\n";

            switch(inst.op)
            {
                case operation::rect:
                {
                    for(size_t i = 0; i < inst.x; ++i)
                    {
                        for(size_t j = 0; j < inst.y; ++j)
                        {
                            screen[j][i] = '#';
                        }
                    }
                }
                    break;
                case operation::rotate:
                {
                    switch(inst.rot)
                    {
                        case rotation::row:
                        {
                            std::array<char, g_width> rotated{};

                            for(size_t i = 0; i < g_width; ++i)
                            {
                                rotated[(i + inst.amount) % g_width] = screen[inst.idx][i];
                            }

                            for(size_t i = 0; i < g_width; ++i)
                            {
                                screen[inst.idx][i] = rotated[i];
                            }
                        }
                            break;
                        case rotation::column:
                        {
                            std::array<char, g_height> rotated{};

                            for(size_t i = 0; i < g_height; ++i)
                            {
                                rotated[(i + inst.amount) % g_height] = screen[i][inst.idx];
                            }

                            for(size_t i = 0; i < g_height; ++i)
                            {
                                screen[i][inst.idx] = rotated[i];
                            }
                        }
                            break;
                    }
                }
                    break;
            }

            ++inst_ptr;
            return true;
        }
        else
        {
            return false;
        }
    }

    auto print_screen() -> void
    {
        for(const auto& row : screen)
        {
            for(const auto& c : row)
            {
                // Makes it a bit easier to read the message in part 2.
                if(c == '.')
                {
                    std::cout << ' ';
                }
                else
                {
                    std::cout << c;
                }
            }
            std::cout << '\n';
        }
    }

    auto lit_pixels() -> uint64_t
    {
        uint64_t lit{0};
        for(const auto& row : screen)
        {
            for(const auto& c : row)
            {
                if(c == '#')
                {
                    ++lit;
                }
            }
        }
        return lit;
    }

    size_t inst_ptr{0};
};

auto parse_rotate(std::string_view data, rotation rot) -> instruction
{
    auto parts = chain::str::split(data, " by ");
    auto offset = parts[0];
    auto amount = chain::str::to_number<uint64_t>(parts[1]).value();
    uint64_t idx = chain::str::to_number<uint64_t>(offset.substr(2)).value();

    return instruction{
        .op = operation::rotate,
        .rot = rot,
        .idx = idx,
        .amount = amount};
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);

    sequence seq{};

    for(auto& line : chain::str::split(contents, '\n'))
    {
        if(chain::str::starts_with(line, "rect"))
        {
            line = line.substr(5);
            auto numbers = chain::str::split(line, 'x');

            seq.instructions.emplace_back(
                instruction{
                    .op = operation::rect,
                    .x = chain::str::to_number<uint64_t>(numbers[0]).value(),
                    .y = chain::str::to_number<uint64_t>(numbers[1]).value()
                }
            );
        }
        else if(chain::str::starts_with(line, "rotate row"))
        {
            line = line.substr(11);
            seq.instructions.emplace_back(parse_rotate(line, rotation::row));
        }
        else if(chain::str::starts_with(line, "rotate column"))
        {
            line = line.substr(14);
            seq.instructions.emplace_back(parse_rotate(line, rotation::column));
        }
        else
        {
            std::cout << "malformed line: " << line << "\n";
        }
    }

    while(seq.advance())
    {
        seq.print_screen();
        std::cout << "lit pixels = " << seq.lit_pixels() << "\n\n";
    }

    return 0;
}
