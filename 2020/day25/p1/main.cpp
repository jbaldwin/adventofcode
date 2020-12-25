#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <optional>

const constexpr int64_t divider{20201227};

auto find_loop_sizes(
    int64_t card_public_key,
    int64_t door_public_key,
    int64_t subject_number) -> std::pair<int64_t, int64_t>
{
    int64_t transformation{1};
    int64_t i{1};

    std::pair<int64_t, int64_t> loops{-1, -1};

    while(loops.first == -1 || loops.second == -1)
    {
        transformation *= subject_number;
        transformation %= divider;

        if(transformation == card_public_key)
        {
            loops.first = i;
        }

        if(transformation == door_public_key)
        {
            loops.second = i;
        }

        ++i;
    }

    return loops;
}

auto transform(int64_t subject_number, int64_t loops) -> int64_t
{
    int64_t transformation{1};
    for(int64_t i = 0; i < loops; ++i)
    {
        transformation *= subject_number;
        transformation %= divider;
    }
    return transformation;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);
    auto lines = chain::str::split(contents, '\n');

    auto card_public_key = chain::str::to_number<int64_t>(lines[0]).value();
    auto door_public_key = chain::str::to_number<int64_t>(lines[1]).value();

    std::cout << "Card public key = " << card_public_key << "\n";
    std::cout << "Door public key = " << door_public_key << "\n";

    auto [card_loop_size, door_loop_size] = find_loop_sizes(card_public_key, door_public_key, 7);

    std::cout << "Card loop size = " << card_loop_size << "\n";
    std::cout << "Door loop size = " << door_loop_size << "\n";

    auto ekey_card = transform(card_public_key, door_loop_size);

    std::cout << "Encryption key = " << ekey_card << "\n";

    return 0;
}
