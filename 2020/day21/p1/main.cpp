#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 0;
    }

    auto contents = file::read(args[1]);

    std::multimap<std::string_view, std::set<std::string_view>> alleries_to_foods{};
    std::set<std::string_view> allergens{};
    std::vector<std::set<std::string_view>> foods{};

    for(auto& line : chain::str::split(contents, '\n'))
    {
        auto all_parts = chain::str::split(line, " (contains ");
        all_parts[1].remove_suffix(1); // strip trailing ')'

        auto ingredients_parts = chain::str::split(all_parts[0], ' ');
        auto allergens_parts = chain::str::split(all_parts[1], ", ");

        std::set<std::string_view> i_set(ingredients_parts.begin(), ingredients_parts.end());

        for(const auto& a : allergens_parts)
        {
            alleries_to_foods.emplace(a, i_set);
            allergens.emplace(a);
        }
        foods.emplace_back(std::move(i_set));
    }

    std::set<std::string_view> determined_allergen{};

    for(const auto& allergen : allergens)
    {
        std::optional<std::string_view> common_ingredient{std::nullopt};
        auto [b1, e1] = alleries_to_foods.equal_range(allergen);
        std::for_each(b1, e1, [&](const auto& pair1)
        {
            const auto& [a, is] = pair1;
            for(const auto& i : is)
            {
                bool common{true};
                auto [b2, e2] = alleries_to_foods.equal_range(allergen);
                std::for_each(b2, e2, [&](const auto& pair2)
                {
                    const auto& [a2, is2] = pair2;
                    if(std::find(is2.begin(), is2.end(), i) == std::end(is2))
                    {
                        common = false;
                        return;
                    }
                });

                if(common)
                {
                    determined_allergen.emplace(i);
                    common_ingredient = i;
                }
            }

            if(common_ingredient.has_value())
            {
                return;
            }
        });

        if(common_ingredient.has_value())
        {
            for(auto& [a, is] : alleries_to_foods)
            {
                is.erase(common_ingredient.value());
            }
        }
    }

    std::cout << "Allergens:\n";
    for(const auto& i : determined_allergen)
    {
        std::cout << i << "\n";
        for(auto& food : foods)
        {
            food.erase(i);
        }
    }
    std::cout << "\n";

    std::map<std::string_view, uint64_t> remaining_counts{};
    for(const auto& food : foods)
    {
        for(const auto& i : food)
        {
            remaining_counts[i]++;
        }
    }

    size_t total{0};
    std::cout << "\nremaining:\n";
    for(const auto& [i, c] : remaining_counts)
    {
        std::cout << i << " = " << c << "\n";
        total += c;
    }
    std::cout << "total = " << total << "\n";

    return 0;
}
