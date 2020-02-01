#pragma once

#include <array>
#include <functional>

namespace algo
{
template<typename DataType, size_t DataLength>
auto _permutate(
    const std::array<DataType, DataLength>& prefix,
    const std::array<DataType, DataLength>& data,
    size_t idx,
    size_t remaining,
    std::function<void(const std::array<DataType, DataLength>&)>& callback
) -> void
{
    if(remaining == 0)
    {
        callback(prefix);
    }
    else
    {
        for(size_t i = 0; i < remaining; ++i)
        {
            std::array<DataType, DataLength> new_prefix = prefix;
            new_prefix[idx] = data[i];

            std::array<DataType, DataLength> new_data{};
            bool i_found{false};
            for(size_t j = 0; j < remaining; ++j)
            {
                if(j == i)
                {
                    i_found = true;
                }
                new_data[j] = (i_found) ? data[j + 1] : data[j];
            }

            _permutate(new_prefix, new_data, idx + 1, remaining - 1, callback);
        }
    }
}

template<typename DataType, size_t DataLength>
auto permutate(
    const std::array<DataType, DataLength>& data,
    std::function<void(const std::array<DataType, DataLength>&)> callback
) -> void
{
    std::array<DataType, DataLength> empty_prefix{};
    _permutate<DataType, DataLength>(empty_prefix, data, 0, DataLength, callback);
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
