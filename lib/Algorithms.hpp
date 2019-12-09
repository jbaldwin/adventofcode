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

} // namespace algo
