#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <list>

#include <lib/file_util.hpp>
#include <chain/chain.hpp>

enum class Techniques
{
    DEAL_INTO_NEW_STACK,
    CUT,
    DEAL_WITH_INCREMENT
};

class Deck
{
public:
    explicit Deck(int64_t num_cards)
    {
        for(int64_t i = 0; i < num_cards; ++i)
        {
            cards.push_back(i);
        }

        swap.resize(num_cards);
    }

    auto Cards() -> std::list<int64_t>&
    {
        return cards;
    }

    auto Cards() const -> const std::list<int64_t>&
    {
        return cards;
    }

    auto DealIntoNewStack() -> void
    {
        std::reverse(cards.begin(), cards.end());
    }

    auto Cut(int64_t amount) -> void
    {
        std::list<int64_t>::iterator start;
        if(amount > 0)
        {
            start = cards.begin();
            std::advance(start, amount);
        }
        else
        {
            start = cards.end();
            std::advance(start, amount);
        }

        cards.splice(
            cards.begin(),
            cards,
            start,
            cards.end()
        );
    }

    auto DealWithIncrement(int64_t inc) -> void
    {
        std::size_t max_size = swap.size();
        std::size_t counter{0};
        for(const auto& c : cards)
        {
            swap[counter % max_size] = c;
            counter += inc;
        }

        std::size_t i{0};
        for(auto& c : cards)
        {
            c = swap[i++];
        }
    }

    friend auto operator<<(std::ostream& os, const Deck& deck) -> std::ostream&
    {
        bool first{true};
        for(const auto& card : deck.Cards())
        {
            if(first)
            {
                first = false;
            }
            else
            {
                os << " ";
            }
            os << card;
        }

        return os;
    }

private:
    std::list<int64_t> cards{};
    std::vector<int64_t> swap{};
};

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 4)
    {
        std::cout << args[0] << " <input_file> <deck_num_cards> <index_to_print>" << std::endl;
        return 1;
    }

    auto contents = file::read(args[1]);
    int64_t num_cards = std::stol(args[2]);
    int64_t index_to_print = std::stol(args[3]);

    Deck deck{num_cards};

    auto lines = chain::str::split(contents, '\n');
    for(const auto& line : lines)
    {
        if(line == "deal into new stack")
        {
            deck.DealIntoNewStack();
            std::cout << "deal into new stack\n";
        }
        else if(line.substr(0, 3) == "cut")
        {
            auto amount = std::stol(std::string{line.substr(4)});
            deck.Cut(amount);
            std::cout << "cut " << amount << '\n';
        }
        else if(line.substr(0, 19) == "deal with increment")
        {
            auto inc = std::stol(std::string{line.substr(20)});
            deck.DealWithIncrement(inc);
            std::cout << "deal with increment " << inc << '\n';
        }
    }

    auto it = deck.Cards().begin();
    std::advance(it, index_to_print);

    std::cout << *it << '\n';

    return 0;
}
