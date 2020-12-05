#include <lib/FileUtil.h>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <cmath>

struct range
{
    uint64_t lower{0};
    uint64_t upper{0};

    auto midpoint() -> uint64_t
    {
        return std::ceil((upper - lower) / 2) + lower;
    }
};

struct boarding_pass
{
    boarding_pass() = default;

    boarding_pass(
        std::string_view row_data,
        std::string_view col_data
    )
    {
        range row_range{0, 127};
        range col_range{0, 7};

        for(size_t i = 0; i < 6; ++i)
        {
            const auto& c = row_data[i];
            switch(c)
            {
                case 'F':
                    row_range.upper = row_range.midpoint();
                    break;
                case 'B':
                    row_range.lower = row_range.midpoint() + 1;
                    break;
            }
        }

        // The last row is special, it just picks the upper or lower bound.
        switch(row_data.back())
        {
            case 'F':
                row_range.upper = row_range.lower;
                break;
            case 'B':
                row_range.lower = row_range.upper;
                break;
        }

        for(size_t i = 0; i < 2; ++i)
        {
            const auto& c = col_data[i];
            switch(c)
            {
                case 'L':
                    col_range.upper = col_range.midpoint();
                    break;
                case 'R':
                    col_range.lower = col_range.midpoint() + 1;
                    break;
            }
        }

        switch(col_data.back())
        {
            case 'L':
                col_range.upper = col_range.lower;
                break;
            case 'R':
                col_range.lower = col_range.upper;
                break;
        }

        row = row_range.lower;
        col = col_range.lower;
    }

    boarding_pass(const boarding_pass&) = default;
    boarding_pass(boarding_pass&&) = default;
    ~boarding_pass() = default;

    auto operator=(const boarding_pass&) noexcept -> boarding_pass& = default;
    auto operator=(boarding_pass&&) noexcept -> boarding_pass& = default;

    uint64_t row{0};
    uint64_t col{0};

    auto seat_id() const -> uint64_t
    {
        return row * 8 + col;
    }

    friend auto operator<<(std::ostream& os, const boarding_pass& bp) -> std::ostream&
    {
        std::cout << "row " << bp.row << ", ";
        std::cout << "col " << bp.col << ", ";
        std::cout << "seat ID " << bp.seat_id();

        return os;
    }

    auto operator>(const boarding_pass& other) const -> bool
    {
        return seat_id() > other.seat_id();
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

    boarding_pass bp_max_seat_id{};

    for(const auto& line : chain::str::split(contents, '\n'))
    {
        auto row_data = line.substr(0, 7);
        auto col_data = line.substr(7);

        boarding_pass bp{row_data, col_data};

        std::cout << line << ": " << bp << ".\n";

        if(bp > bp_max_seat_id)
        {
            bp_max_seat_id = bp;
        }
    }

    std::cout << "Highest seat ID " << bp_max_seat_id.seat_id() << "\n";

    return 0;
}
