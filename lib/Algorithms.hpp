#pragma once

#include <array>
#include <functional>

namespace algo
{

template<typename ContainerType>
auto _permutate(
    ContainerType& data,
    std::size_t k,
    std::function<void(const ContainerType&)>& callback
)
{
    if(k == 1)
    {
        callback(data);
        return;
    }

    _permutate(data, k - 1, callback);

    bool even = (k % 2) == 0;
    for(std::size_t i = 0; i < k - 1; ++i)
    {
        if(even)
        {
            std::swap(data[i], data[k - 1]);
        }
        else
        {
            std::swap(data[0], data[k - 1]);
        }

        _permutate(data, k - 1, callback);
    }
}

template<typename ContainerType>
auto permutate(
    ContainerType data,
    std::function<void(const ContainerType&)> callback
) -> void
{
    // https://en.wikipedia.org/wiki/Heap's_algorithm
    _permutate<ContainerType>(data, std::size(data), callback);
}

template<typename ContainerType1, typename ContainerType2>
auto _combinate(
    const ContainerType1& data,
    std::function<void(const ContainerType2&)> callback,
    std::size_t len,
    std::size_t start_pos,
    ContainerType2& result
) -> void
{
    if(len == 0)
    {
        callback(result);
        return;
    }

    for(std::size_t i = start_pos; i <= data.size() - len; ++i)
    {
        result[result.size() - len] = data[i];
        _combinate(data, callback, len - 1, i + 1, result);
    }
}

template<typename ContainerType1, typename ContainerType2>
auto combinate(
    const ContainerType1& data,
    std::size_t combo_size,
    std::function<void(const ContainerType2&)> callback
) -> void
{
    ContainerType2 result{};
    _combinate(data, callback, combo_size, 0, result);
}

/// Calculates the greatest common divisor of \c a and \c b.
/// \tparam IntType An integer type.
template<typename IntType>
auto gcd(
    IntType a,
    IntType b
) -> IntType
{
    while(b != 0)
    {
        IntType t = a;
        a = b;
        b = t % b;
    }
    return a;
}

template<typename IntType, typename... ArgType>
auto gcd(
    IntType a,
    ArgType... args
) -> IntType
{
    return gcd(a, gcd(args...));
}

/// Calculates the least common multiple of \c a and \c b.
/// \tparam IntType An integer type.
template<typename IntType>
auto lcm(
    IntType a,
    IntType b
) -> IntType
{
    return (a / gcd(a, b)) * b;
}

template<typename IntType, typename... ArgType>
auto lcm(
    IntType a,
    ArgType... args
) -> IntType
{
    return lcm(a, lcm(args...));
}

/// Reduces the fraction of \c n / \c d.
/// \tparam IntType An integer type.
template<typename IntType>
auto fraction_reduce(
    IntType& n,
    IntType& d
) -> void
{
    auto div = gcd(n, d);

    n /= div;
    d /= div;
}

} // namespace algo
