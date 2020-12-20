#pragma once

#include <tuple>
#include <stdexcept>
#include <iostream>

namespace containers
{

template<typename type>
struct vec2
{
    type x{0};
    type y{0};

    vec2() = default;

    vec2(
        type x_param,
        type y_param
    )
        :   x(x_param)
        ,   y(y_param)
    {

    }

    auto operator[](size_t i) -> type&
    {
        switch(i)
        {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range(std::to_string(i));
        }
    }

    auto operator[](size_t i) const -> const type&
    {
        switch(i)
        {
            case 0: return x;
            case 1: return y;
            default: throw std::out_of_range(std::to_string(i));
        }
    }

    auto operator==(const vec2& other) const -> bool
    {
        return x == other.x && y == other.y;
    }

    auto operator<(const vec2& other) const -> bool
    {
        return std::tie(x, y) < std::tie(other.x, other.y);
    }

    auto operator+(const vec2& other) const -> vec2
    {
        return vec2{x + other.x, y + other.y};
    }

    auto operator+=(const vec2& other) -> void
    {
        x += other.x;
        y += other.y;
    }

    auto operator-(const vec2& other) const -> vec2
    {
        return vec2{x - other.x, y - other.y};
    }

    auto operator+=(const vec2& other) const -> void
    {
        x -= other.x;
        y -= other.y;
    }

    friend auto operator<<(std::ostream& os, const vec2& v) -> std::ostream&
    {
        os << "[";
        os << "x=" << v.x;
        os << ", y=" << v.y;
        os << "]";
        return os;
    }
};

template<typename type>
struct vec3
{
    type x{0};
    type y{0};
    type z{0};

    vec3() = default;

    vec3(
        type x_param,
        type y_param,
        type z_param
    )
        : x(x_param)
        , y(y_param)
        , z(z_param)
    {

    }

    auto operator[](size_t i) -> type&
    {
        switch(i)
        {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range(std::to_string(i));
        }
    }

    auto operator[](type i) const -> const type&
    {
        switch(i)
        {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: throw std::out_of_range(std::to_string(i));
        }
    }

    auto operator==(const vec3& other) const -> bool
    {
        return x == other.x && y == other.y && z == other.z;
    }

    auto operator<(const vec3& other) const -> bool
    {
        return std::tie(x, y, z) < std::tie(other.x, other.y, other.z);
    }

    auto operator+(const vec3& other) const -> vec3
    {
        return vec3{x + other.x, y + other.y, z + other.z};
    }

    auto operator+=(const vec3& other) -> void
    {
        x += other.x;
        y += other.y;
        z += other.z;
    }

    friend auto operator<<(std::ostream& os, const vec3& v) -> std::ostream&
    {
        os << "[";
        os << "x=" << v.x;
        os << ", y=" << v.y;
        os << ", z=" << v.z;
        os << "]";
        return os;
    }
};

template<typename type>
class vec4
{
public:

    type x{0};
    type y{0};
    type z{0};
    type w{0};

    vec4() = default;

    vec4(
        type x_param,
        type y_param,
        type z_param,
        type w_param
    ) : x(x_param),
        y(y_param),
        z(z_param),
        w(w_param)
    {

    }

    auto operator==(const vec4& other) const -> bool
    {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    friend auto operator<<(std::ostream& os, const vec4& v) -> std::ostream&
    {
        os << "[";
        os << "x=" << v.x;
        os << ", y=" << v.y;
        os << ", z=" << v.z;
        os << ", w=" << v.w;
        os << "]";
        return os;
    }
};


} // namepsace containers
