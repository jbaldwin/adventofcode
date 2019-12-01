#include <string_view>
#include <vector>

namespace str
{

auto split(
    std::string_view s,
    std::string_view delim,
    std::vector<std::string_view>& out) -> void
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
    std::string_view delim) -> std::vector<std::string_view>
{
    std::vector<std::string_view> out{};
    split(s, delim, out);
    return out;
}



} // namespace str
