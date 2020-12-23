#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <set>

struct player
{
    int64_t id;
    std::deque<int64_t> cards;

    auto score() const -> uint64_t
    {
        uint64_t multiplier = cards.size();
        uint64_t sum{0};
        for(const auto& c : cards)
        {
            sum += c * multiplier;
            --multiplier;
        }
        return sum;
    }

    auto to_string_key() const -> std::string
    {
        return chain::str::join(cards, ',');
    }

    friend auto operator<<(std::ostream& os, const player& p) -> std::ostream&
    {
        os << "Player " << p.id << "'s deck: " << chain::str::join(p.cards, ", ");
        return os;
    }
};

enum winner
{
    p1 = 1,
    p2 = 2
};

auto to_string(const winner& w) -> char
{
    switch(w)
    {
        case winner::p1: return '1';
        case winner::p2: return '2';
    }
    throw std::runtime_error{"invalid"};
}

auto play_recursive_combat(std::array<player, 2>& players, uint64_t& game_num) -> winner
{
    uint64_t my_game_num = game_num;
    std::cout << "=== Game " << game_num << " ===\n\n";
    std::set<std::string> rounds_played{};

    auto& p1 = players[0];
    auto& p2 = players[1];

    int64_t round{1};

    while(!p1.cards.empty() && !p2.cards.empty())
    {
        std::cout << "-- Round " << round << "(Game " << my_game_num << ") --\n";
        std::cout << p1 << "\n";
        std::cout << p2 << "\n";

        auto key = p1.to_string_key() + "|" + p2.to_string_key();
        if(rounds_played.emplace(key).second)
        {
            winner w{winner::p1};

            auto p1_card = p1.cards.front();
            auto p2_card = p2.cards.front();

            p1.cards.pop_front();
            p2.cards.pop_front();

            std::cout << "Player 1 plays: " << p1_card << "\n";
            std::cout << "Player 2 plays: " << p2_card << "\n";

            if(p1_card <= p1.cards.size() && p2_card <= p2.cards.size())
            {
                // play another round of recursive combat

                std::array<player, 2> sub_players = players;
                sub_players[0].cards.resize(p1_card);
                sub_players[1].cards.resize(p2_card);

                ++game_num;
                w = play_recursive_combat(sub_players, game_num);
                std::cout << "...anyway, back to game " << my_game_num << "\n";

            }
            else
            {
                w = (p1_card > p2_card) ? winner::p1 : winner::p2;
            }

            std::cout << "Player " << to_string(w) << " wins round " << round << " of game " << my_game_num << "!\n\n";

            switch(w)
            {
                case winner::p1:
                    p1.cards.emplace_back(p1_card);
                    p1.cards.emplace_back(p2_card);
                    break;
                case winner::p2:
                    p2.cards.emplace_back(p2_card);
                    p2.cards.emplace_back(p1_card);
                    break;
            }
        }
        else
        {
            std::cout << "Key " << key << " has been seen before, player 1 wins.\n";
            return winner::p1;
        }

        ++round;
    }

    auto game_winner = (!p1.cards.empty()) ? winner::p1 : winner::p2;

    std::cout << "The winner of game " << my_game_num << " is player " << to_string(game_winner) << "!\n";

    return game_winner;
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

    auto player_parts = chain::str::split(contents, "Player ");

    std::array<player, 2> players{};

    for(auto& player_data : player_parts)
    {
        if(player_data.empty())
        {
            continue;
        }

        player p;
        p.id = chain::str::to_number<int64_t>(player_data).value();
        player_data.remove_prefix(3);

        for(const auto& card_data : chain::str::split(player_data, '\n'))
        {
            if(card_data.empty())
            {
                continue;
            }

            p.cards.emplace_back(chain::str::to_number<int64_t>(card_data).value());
        }

        players[p.id - 1] = p;
    }

    uint64_t game_num{1};
    auto w = play_recursive_combat(players, game_num);

    std::cout << "\n";
    std::cout << players[0] << "\n";
    std::cout << players[1] << "\n";
    std::cout << "Player 1's score = " << players[0].score() << "\n";
    std::cout << "Player 2's score = " << players[1].score() << "\n";


    return 0;
}
