#include <iostream>
#include <vector>
#include <string>
#include <optional>

#include <lib/FileUtil.h>
#include <chain/Chain.hpp>

struct Item
{
    Item(
        std::string _name,
        int64_t _cost,
        int64_t _damage,
        int64_t _armor
    )
        : name(std::move(_name))
        , cost(_cost)
        , damage(_damage)
        , armor(_armor)
    {
        
    }

    std::string name{};
    int64_t cost{0};
    int64_t damage{0};
    int64_t armor{0};
};

struct Character
{
    Character(
        std::string _name,
        int64_t _hit_points,
        Item _weapon,
        Item _armor,
        Item _left_ring,
        Item _right_ring
    )
        : name(std::move(_name))
        , hit_points(_hit_points)
        , weapon(std::move(_weapon))
        , armor(std::move(_armor))
        , left_ring(std::move(_left_ring))
        , right_ring(std::move(_right_ring))
    {
        
    }

    auto Damage() const -> int64_t
    {
        return weapon.damage + left_ring.damage + right_ring.damage;
    }

    auto Armor() const -> int64_t
    {
        return armor.armor + left_ring.armor + right_ring.armor;
    }

    auto Cost() const -> int64_t
    {
        int64_t cost{0};
        cost += weapon.cost;
        cost += armor.cost;
        cost += left_ring.cost;
        cost += right_ring.cost;
        return cost;
    }

    friend auto operator<<(std::ostream& os, const Character& c) -> std::ostream&
    {
        os << "{";
        os << c.name << ",";
        os << "hp=" << c.hit_points << ",";
        os << "weapon=" << c.weapon.name << ",";
        os << "armor=" << c.armor.name << ",";
        os << "left_ring=" << c.left_ring.name << ",";
        os << "right_ring=" << c.right_ring.name << ",";
        os << "cost=" << c.Cost();
        os << "}";
        return os;
    }

    std::string name{};
    int64_t hit_points{0};
    Item weapon;
    Item armor;
    Item left_ring;
    Item right_ring;
};

auto attack(
    Character& attacker,
    Character& defender,
    bool print
) -> void
{
    auto d = attacker.Damage();
    auto a = defender.Armor();

    auto hit = d - a;
    if(hit <= 0)
    {
        hit = 1;
    }

    defender.hit_points -= hit;

    if(print)
    {
        std::cout << "The " << attacker.name << " deals ";
        std::cout << d << "-" << a << " = " << hit << " damage; ";
        std::cout << "the " << defender.name << " goes down to ";
        std::cout << defender.hit_points << " hit points.\n";
    }
}

auto fight(
    Character player,
    Character boss,
    bool print = false
) -> bool
{
    bool players_turn{true};
    while(true)
    {
        if(players_turn)
        {
            attack(player, boss, print);

            if(boss.hit_points <= 0)
            {
                return true;
            }
        }
        else
        {
            attack(boss, player, print);

            if(player.hit_points <= 0)
            {
                return false;
            }
        }

        players_turn = !players_turn;
    }
}

int main(int argc, char* argv[])
{
    const std::vector<Item> weapons
    {
        Item{"Dagger",      8,  4, 0},
        Item{"Shortsword",  10, 5, 0},
        Item{"Warhammer",   25, 6, 0},
        Item{"Longsword",   40, 7, 0},
        Item{"Greataxe",    74, 8, 0}
    };
    const std::vector<Item> armors
    {
        Item{"None",        0,   0, 0},
        Item{"Leather",     13,  0, 1},
        Item{"Chainmail",   31,  0, 2},
        Item{"Splintmail",  53,  0, 3},
        Item{"Bandedmail",  75,  0, 4},
        Item{"Platemail",   102, 0, 5}
    };
    const std::vector<Item> rings
    {
        Item{"None1",       0,   0, 0},
        Item{"None2",       0,   0, 0},
        Item{"Damage + 1",  25,  1, 0},
        Item{"Damage + 2",  50,  2, 0},
        Item{"Damage + 3",  100, 3, 0},
        Item{"Defense + 1", 20,  0, 1},
        Item{"Defense + 2", 40,  0, 2},
        Item{"Defense + 3", 80,  0, 3},
    };

    // Example
    const Character example_player{"player", 8, weapons[1], armors[5], rings[0], rings[1]};
    const Character example_boss{"boss", 12, weapons[3], armors[2], rings[0], rings[1]};

    if(fight(example_player, example_boss, true))
    {
        std::cout << "Player wins.\n\n";
    }
    else
    {
        std::cout << "Boss wins.\n\n";
    }

    // Hit Points: 104
    // Damage: 8
    // Armor: 1
    const Character boss{
        "boss", 
        104, 
        Item{"Greataxe",    74,  8, 0}, 
        Item{"Leather",     3,   0, 1},
        Item{"None1",       0,   0, 0},
        Item{"None2",       0,   0, 0},
    };

    // Run part1 and part2 at the same time.
    int64_t min_cost = std::numeric_limits<int64_t>::max();
    int64_t max_cost = 0;

    for(const auto& weapon : weapons)
    {
        for(const auto& armor: armors)
        {
            for(const auto& left_ring : rings)
            {
                for(const auto& right_ring : rings)
                {
                    if(right_ring.name == left_ring.name)
                    {
                        // Can't wear the same ring on each hand
                        continue;
                    }

                    Character player{
                        "player",
                        100, // player always has 100 hp
                        weapon,
                        armor,
                        left_ring,
                        right_ring
                    };

                    if(fight(player, boss))
                    {
                        auto cost = player.Cost();
                        if(cost < min_cost)
                        {
                            min_cost = cost;
                            std::cout << "Min cost = " << player << "\n";
                        }
                    }
                    else
                    {
                        auto cost = player.Cost();
                        if(cost > max_cost)
                        {
                            max_cost = cost;
                            std::cout << "Max cost = " << player << "\n";
                        }
                    }
                }
            }
        }
    }

    std::cout << "Min cost = " << min_cost << "\n";
    std::cout << "Max cost = " << max_cost << "\n";

    return 0;
}
