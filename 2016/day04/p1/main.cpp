#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <map>

int main(int argc, char* argv[])
{
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

    uint64_t sector_id_sum{0};

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

        std::cout << "[" << expected_checksum << "] [" << actual_checksum << "] ";
        std::cout << "sector_id=[" << sector_id << "] ";

        // Is this a real room?
        if(expected_checksum == actual_checksum)
        {
            std::cout << "real room\n";
            sector_id_sum += sector_id;
        }
        else
        {
            std::cout << "fake room\n";
        }
    }

    std::cout << "sector_id sum = [" << sector_id_sum << "]\n";

    return 0;
}
