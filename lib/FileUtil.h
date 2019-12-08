#pragma once

#include <string_view>
#include <string>

namespace file
{

auto read(
    std::string_view file_name) -> std::string;

} // namespace file
