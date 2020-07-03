#pragma once

#include <tuple>

namespace containers
{

template<typename IntType>
struct Vec2
{
    IntType x{0};
    IntType y{0};

    Vec2(
        IntType x_param,
        IntType y_param
    )
        :   x(x_param)
        ,   y(y_param)
    {

    }

    auto operator[](size_t i) -> IntType&
    {
        switch(i)
        {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range(std::to_string(i));
        }
    }

    auto operator[](size_t i) const -> const IntType&
    {
        switch(i)
        {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range(std::to_string(i));
        }
    }

    auto operator==(const Vec2& other) const -> bool
    {
        return x == other.x && y == other.y;
    }

    auto operator<(const Vec2& other) const -> bool
    {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }

    auto operator+(const Vec2& other) const -> Vec2
    {
        return Vec2{x + other.x, y + other.y};
    }

    auto operator+=(const Vec2& other) -> void
    {
        x += other.x;
        y += other.y;
    }

    friend auto operator<<(std::ostream& os, const Vec2& v) -> std::ostream&
    {
        os << "[";
        os << "x=" << v.x;
        os << ", y=" << v.y;
        os << "]";
        return os;
    }
};

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

    auto operator<(const Vec3& other) const -> bool
    {
        return std::tie(x, y, z) < std::tie(other.x, other.y, other.z);
    }

    friend auto operator<<(std::ostream& os, const Vec3& v) -> std::ostream&
    {
        os << "[";
        os << "x=" << v.x;
        os << ", y=" << v.y;
        os << ", z=" << v.z;
        os << "]";
        return os;
    }
};
} // namepsace containers
