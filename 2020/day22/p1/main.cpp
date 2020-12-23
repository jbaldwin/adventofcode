#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <queue>

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

    friend auto operator<<(std::ostream& os, const player& p) -> std::ostream&
    {
        os << "Player " << p.id << ":\n";
        for(const auto& c : p.cards)
        {
            std::cout << c << "\n";
        }
        return os;
    }
};

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

    size_t rounds{0};
    while(!players[0].cards.empty() && !players[1].cards.empty())
    {
        auto p1_card = players[0].cards.front();
        auto p2_card = players[1].cards.front();

        players[0].cards.pop_front();
        players[1].cards.pop_front();

        if(p1_card > p2_card)
        {
            players[0].cards.push_back(p1_card);
            players[0].cards.push_back(p2_card);
        }
        else
        {
            players[1].cards.push_back(p2_card);
            players[1].cards.push_back(p1_card);
        }

        ++rounds;
    }

    std::cout << "Rounds: " << rounds << "\n";

    for(const auto& p : players)
    {
        std::cout << p << "\n";
        std::cout << "Score: " << p.score() << "\n\n";
    }

    return 0;
}
