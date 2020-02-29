#include <iostream>
#include <vector>
#include <string>
#include <map>

#include <lib/FileUtil.h>
#include <lib/StringUtil.h>

struct Ingredient
{
    std::string name;
    int64_t capacity{0};
    int64_t durability{0};
    int64_t flavor{0};
    int64_t texture{0};
    int64_t calories{0};

    auto score_capacity(int64_t amount) const -> int64_t
    {
        return amount * capacity;
    }

    auto score_durability(int64_t amount) const -> int64_t
    {
        return amount * durability;
    }

    auto score_flavor(int64_t amount) const -> int64_t
    {
        return amount * flavor;
    }

    auto score_texture(int64_t amount) const -> int64_t
    {
        return amount * texture;
    }

    auto score_calories(int64_t amount) const -> int64_t
    {
        return amount * calories;
    }

    friend auto operator<<(std::ostream& os, const Ingredient& i) -> std::ostream&
    {
        std::cout << "{";
        std::cout << i.name;
        std::cout << " capacity=" << i.capacity;
        std::cout << " durability=" << i.durability;
        std::cout << " flavor=" << i.flavor;
        std::cout << " texture=" << i.texture;
        std::cout << " calories=" << i.calories;
        std::cout << "}";

        return os;
    }
};

struct Ingredients
{
    std::vector<Ingredient> ingredients{};

    auto size() const -> std::size_t
    {
        return ingredients.size();
    }

    auto push_back(Ingredient i) -> void
    {
        ingredients.push_back(i);
    }

    auto score_calories(const std::vector<int64_t>& amounts) const -> int64_t
    {
        int64_t calories{0};

        for(std::size_t i = 0 ; i < ingredients.size(); ++i)
        {
            const auto& ing = ingredients[i];
            auto amount = amounts[i];

            calories += ing.score_calories(amount);
        }

        if(calories < 0) return 0;
        return calories;
    }

    auto score(const std::vector<int64_t>& amounts) const -> int64_t 
    {
        int64_t capacity{0};
        int64_t durability{0};
        int64_t flavor{0};
        int64_t texture{0};

        for(std::size_t i = 0; i < ingredients.size(); ++i)
        {
            const auto& ing = ingredients[i];
            auto amount = amounts[i];

            capacity += ing.score_capacity(amount);
            durability += ing.score_durability(amount);
            flavor += ing.score_flavor(amount);
            texture += ing.score_texture(amount);
        }

        // If any properties produce a negative total its zero.
        if(     capacity < 0
            ||  durability < 0
            ||  flavor < 0
            ||  texture < 0
        )
        {
            return 0;
        }

        int64_t score = capacity * durability * flavor * texture;
        if(score < 0) score = 0;
        return score;
    }
};

auto choose(
    const Ingredients& ingredients,
    std::vector<int64_t>& amounts,
    int64_t amount_remaining,
    int64_t target_calories,
    std::map<std::string, int64_t>& cache
) -> int64_t
{
    if(amount_remaining == 0)
    {
        auto calories = ingredients.score_calories(amounts);
        if(calories == target_calories)
        {
            auto score = ingredients.score(amounts);
            if(score > 0)
            {
                for(const auto& amount : amounts)
                {
                    std::cout << amount << ", ";
                }
                std::cout << score << '\n';
            }
            return score;
        }
        else
        {
            return 0;
        }
    }

    std::string cache_key{};
    for(const auto& i : amounts)
    {
        cache_key += std::to_string(i);
        cache_key += '-';
    }

    if(auto found = cache.find(cache_key); found != cache.end())
    {
        return found->second;
    }

    int64_t max{0};
    for(std::size_t i = 0; i < ingredients.size(); ++i)
    {
        amounts[i]++;
        max = std::max(max, choose(ingredients, amounts, amount_remaining - 1, target_calories, cache));
        amounts[i]--;
    }

    cache[cache_key] = max;

    return max;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    auto contents = file::read(args[1]);
    auto lines = str::split(contents, '\n');

    Ingredients ingredients{};

    for(const auto& line : lines)
    {
        auto parts = str::split(line, ' ');

        Ingredient i{};

        parts[0].remove_suffix(1);
        i.name = std::string{parts[0]};

        i.capacity = std::stol(std::string{parts[2]});
        i.durability = std::stol(std::string{parts[4]});
        i.flavor = std::stol(std::string{parts[6]});
        i.texture = std::stol(std::string{parts[8]});
        i.calories = std::stol(std::string{parts[10]});

        ingredients.push_back(std::move(i));
    }

    std::vector<int64_t> amounts{};
    amounts.resize(ingredients.size());
    std::map<std::string, int64_t> cache{};

    auto score = choose(ingredients, amounts, 100, 500, cache);

    std::cout << "Top score " << score << '\n';

    return 0;
}