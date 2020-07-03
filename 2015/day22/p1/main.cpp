#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <set>
#include <map>

#include <lib/FileUtil.h>
#include <chain/Chain.hpp>

struct Spell
{
    Spell(
        std::string _name,
        int64_t _mana_cost,
        int64_t _hp,
        int64_t _mp,
        int64_t _dmg,
        int64_t _armor,
        int64_t _duration
    )
        : name(std::move(_name))
        , mana_cost(_mana_cost)
        , hp(_hp)
        , mp(_mp)
        , dmg(_dmg)
        , armor(_armor)
        , duration(_duration)
    {

    }

    std::string name;
    int64_t mana_cost;
    int64_t hp;
    int64_t mp;
    int64_t dmg;
    int64_t armor;
    int64_t duration;
};

struct Character
{
    Character(
        std::string _name,
        int64_t _hit_points,
        int64_t _mana_points,
        int64_t _damage
    )
        : name(std::move(name))
        , hit_points(_hit_points)
        , mana_points(_mana_points)
        , damage(_damage)
    {

    }

    std::string name;
    int64_t hit_points{0};
    int64_t mana_points{0};
    int64_t armor{0};
    int64_t damage{0};
};

static int64_t g_min_mana_used = std::numeric_limits<int64_t>::max();

auto fight(
    Character player,
    Character boss,
    const std::vector<Spell>& spells,
    std::map<std::string, Spell> active_spells,
    int64_t mana_used,
    bool players_turn,
    bool hard_mode
) -> void
{
    if(hard_mode && players_turn)
    {
        player.hit_points--;
        if(player.hit_points <= 0)
        {
            return;
        }
    }

    player.armor = 0; // Reset so it doesn't continuously apply armor via Shield

    // Apply active spells
    {
        std::map<std::string, Spell> active_spells_copy{};
        for(auto& [active_spell_name, active_spell] : active_spells)
        {
            player.hit_points += active_spell.hp;
            player.mana_points += active_spell.mp;
            boss.hit_points -= active_spell.dmg;
            player.armor += active_spell.armor;

            active_spell.duration--;
            if(active_spell.duration > 0)
            {
                active_spells_copy.emplace(active_spell_name, active_spell);
            }
        }
        active_spells.swap(active_spells_copy);
    }

    if(boss.hit_points <= 0)
    {
        if(mana_used < g_min_mana_used)
        {
            g_min_mana_used = mana_used;
        }
        return;
    }

    // No point exploring trees that cost too much mana.
    if(mana_used > g_min_mana_used)
    {
        return;
    }

    if(players_turn)
    {
        for(const auto& spell : spells)
        {
            if(auto in_use = active_spells.find(spell.name); in_use == active_spells.end())
            {
                if(spell.mana_cost <= player.mana_points)
                {
                    auto new_active_spells = active_spells;
                    new_active_spells.emplace(spell.name, spell);

                    auto new_player = player;
                    new_player.mana_points -= spell.mana_cost;

                    fight(
                        std::move(new_player),
                        boss,
                        spells, 
                        std::move(new_active_spells), 
                        mana_used + spell.mana_cost, 
                        false,
                        hard_mode
                    );
                }
            }
        }
    }
    else
    {
        player.hit_points -= boss.damage - player.armor;
        if(player.hit_points <= 0)
        {
            return;
        }

        fight(player, boss, spells, std::move(active_spells), mana_used, true, hard_mode);
    }
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
    auto parts = chain::str::split(contents, '\n');

    auto boss_hp     = std::stol(std::string{chain::str::split(parts[0], ':')[1]});
    auto boss_damage = std::stol(std::string{chain::str::split(parts[1], ':')[1]});

    auto player_hp   = std::stol(std::string{chain::str::split(parts[2], ':')[1]});
    auto player_mana = std::stol(std::string{chain::str::split(parts[3], ':')[1]});

    std::cout << "Boss hp " << boss_hp << " boss damage " << boss_damage << "\n";
    std::cout << "Player hp " << player_hp << " player mana " << player_mana << "\n";

    const std::vector<Spell> spells
    {
        //                    cost hp mp dmg arm duration
        Spell{"Magic Missle",   53, 0, 0, 4, 0, 0},
        Spell{"Drain",          73, 2, 0, 2, 0, 0},
        Spell{"Shield",        113, 0, 0, 0, 7, 6},
        Spell{"Poison",        173, 0, 0, 3, 0, 6},
        Spell{"Recharge",      229, 0, 101, 0, 0, 5}
    };

    const Character boss{"Boss", boss_hp, 0, boss_damage};
    const Character player{"Player", player_hp, player_mana, 0};

    fight(player, boss, spells, {}, 0, true, false);
    std::cout << "Min mana used " << g_min_mana_used << "\n";

    g_min_mana_used = std::numeric_limits<int64_t>::max();
    fight(player, boss, spells, {}, 0, true, true);
    std::cout << "Min mana used (HARD) " << g_min_mana_used << "\n";

    // Harder example spells casts for minimum mana used.
    // recharge
    // shield
    // drain
    // poison
    // magic missle

    return 0;
}
