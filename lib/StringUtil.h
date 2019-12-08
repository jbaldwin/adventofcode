#pragma once

#include <string_view>
#include <vector>
#include <string>

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
    std::string_view s) -> std::string_view;

} // namespace str
