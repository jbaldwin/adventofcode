#pragma once

namespace containers
{

template<typename IntType>
struct Vec3
{
    IntType x{0};
    IntType y{0};
    IntType z{0};

    auto operator[](size_t i) -> IntType&
    {
        switch(i)
        {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range(std::to_string(i));
        }
    }

    auto operator[](IntType i) const -> const IntType&
    {
        switch(i)
        {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range(std::to_string(i));
        }
    }

    auto operator==(const Vec3& other) const -> bool
    {
        return x == other.x && y == other.y && z == other.z;
    }

    friend auto operator<<(std::ostream& os, const Vec3& v) -> std::ostream&
    {
        os << "[";
        os << "x=" << v.x;
        os << ", y=" << v.y;
        os << ", z=" << v.z;
        os << "]";
    }
};
} // namepsace containers
