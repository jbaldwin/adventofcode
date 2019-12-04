#pragma once

#include <string_view>
#include <vector>
#include <algorithm>

namespace str
{

auto split(
    std::string_view s,
    std::string_view delim,
    std::vector<std::string_view>& out
    ) -> void
{
    size_t length;
    size_t start = 0;

    while (true)
    {
        size_t next = s.find(delim, start);
        if (next == std::string_view::npos)
        {
            // The length of this split is the full string length - start.
            // This is also true if there were no delimiters found at all.
            length = s.length() - start;
            out.emplace_back(s.data() + start, length);
            break;
        }

        // The length of this split is from start to next.
        length = next - start;
        out.emplace_back(s.data() + start, length);

        // Update our iteration to skip the 'next' delimiter found.
        start = next + delim.length();
    }
}

auto split(
    std::string_view s,
    std::string_view delim
    ) -> std::vector<std::string_view>
{
    std::vector<std::string_view> out{};
    split(s, delim, out);
    return out;
}

auto split(
    std::string_view s,
    char delim
    ) -> std::vector<std::string_view>
{
    std::vector<std::string_view> out{};
    split(s, std::string_view{&delim, 1}, out);
    return out;
}

auto to_lower(
    std::string& data) -> void
{
    std::transform(data.begin(), data.end(), data.begin(), ::tolower);
}

auto to_upper(
    std::string& data) -> void
{
    std::transform(data.begin(), data.end(), data.begin(), ::toupper);
}

auto trim_left(
    std::string_view s
    ) -> std::string_view
{
    if (s.length() == 0)
    {
        return s;
    }

    size_t index = 0;
    while (index < s.length())
    {
        if (!std::isspace(s[index]))
        {
            break;
        }
        else
        {
            ++index;
        }
    }
    return s.substr(index);
}

auto trim_right(
    std::string_view s
    ) -> std::string_view
{
    if (s.length() == 0)
    {
        return s;
    }

    int64_t index = static_cast<int64_t>(s.length() - 1);
    while (index >= 0)
    {
        if (!std::isspace(s[static_cast<size_t>(index)]))
        {
            break;
        }
        else
        {
            --index;
        }
    }
    return s.substr(0, static_cast<size_t>(index + 1));
}

auto trim(
    std::string_view s) -> std::string_view
{
    return trim_left(trim_right(s));
}

} // namespace str
