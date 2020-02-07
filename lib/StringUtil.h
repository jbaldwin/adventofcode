#pragma once

#include <string_view>
#include <vector>
#include <string>
#include <sstream>

namespace str
{

auto split(
    std::string_view s,
    std::string_view delim,
    std::vector<std::string_view>& out
    ) -> void;

auto split(
    std::string_view s,
    std::string_view delim
    ) -> std::vector<std::string_view>;

auto split(
    std::string_view s,
    char delim
    ) -> std::vector<std::string_view>;

template<typename ContainerType>
auto join(
    const ContainerType& items,
    std::string_view delim
    ) -> std::string
{
    thread_local std::stringstream ss{};

    ss.clear();
    ss.str("");

    auto size = std::size(items);

    bool first{true};
    for(const auto& item : items)
    {
        if(first)
        {
            first = false;
        }
        else
        {
            ss << delim;
        }
        ss << item;
    }

    return ss.str();
}

template<typename ContainerType, typename CallbackType>
auto map_join(
    const ContainerType& items,
    std::string_view delim,
    const CallbackType& transform
    ) -> std::string
{
    thread_local std::stringstream ss{};

    ss.clear();
    ss.str("");

    auto size = std::size(items);

    bool first{true};
    for(const auto& item : items)
    {
        if(first)
        {
            first = false;
        }
        else
        {
            ss << delim;
        }
        ss << transform(item);
    }

    return ss.str();
}

auto to_lower(
    std::string& data) -> void;

auto to_upper(
    std::string& data) -> void;

auto trim_left(
    std::string_view s
    ) -> std::string_view;

auto trim_right(
    std::string_view s
    ) -> std::string_view;

auto trim(
    std::string_view s
    ) -> std::string_view;

} // namespace str
