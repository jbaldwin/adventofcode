#include <lib/FileUtil.h>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <map>

static auto rotate_letter(char c, size_t amount) -> char
{
    amount %= 26;
    c += amount;

    // If overflowed subtract maximum letter amount.
    return (c > 'z') ? (c - 26) : c;
}

int main(int argc, char* argv[])
{
    std::string expected_message{"northpole object storage "};

    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);

    std::vector<std::string_view> rooms{};
    for(const auto& line : chain::str::split(contents, '\n'))
    {
        rooms.push_back(line);
    }

    for(const auto& room : rooms)
    {
        std::map<char, uint64_t> letters{};

        auto parts = chain::str::split(room, "[");
        auto encrypted_name = parts[0];
        auto expected_checksum = parts[1];
        expected_checksum.remove_suffix(1); // drop ']'

        auto encrypted_parts = chain::str::split(encrypted_name, '-');

        auto sector_id = chain::str::to_number<uint64_t>(encrypted_parts.back()).value_or(0);
        encrypted_parts.pop_back();

        // Count each letter.
        for(const auto& encrypted_part : encrypted_parts)
        {
            for(const auto& c : encrypted_part)
            {
                letters[c]++;
            }
        }

        // Now sort each letter from greatest to least.
        std::multimap<uint64_t, char, std::greater<>> ordered{};
        for(const auto& [c, i] : letters)
        {
            ordered.emplace(i, c);
        }

        // Generate the maximum 5 character checksum.
        std::string actual_checksum;

        size_t stop{0};
        for(const auto& [i, c] : ordered)
        {
            if(stop >= 5)
            {
                break;
            }
            ++stop;
            actual_checksum += c;
        }

        // Is this a real room?
        if(expected_checksum == actual_checksum)
        {
            std::string message;
            for(const auto& encrypted_part : encrypted_parts)
            {
                for(const auto& c : encrypted_part)
                {
                    message += rotate_letter(c, sector_id);
                }
                message += ' '; // this will leave a dangling space at the end of the message, thats ok.
            }

            if(message == expected_message)
            {
                std::cout << message << "sector_id=[" << sector_id << "]\n";
                return 0;
            }
        }
    }

    return 0;
}
