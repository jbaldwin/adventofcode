#include "lib/file_util.hpp"

#include <fstream>

namespace file
{

auto read(
    std::string_view file_name) -> std::string
{
    std::ifstream file(file_name.data(), std::ios::ate); // open at end
    auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string buffer{};
    buffer.resize(size, '\0');

    file.read(buffer.data(), size);

    return buffer;
}

} // namespace file
