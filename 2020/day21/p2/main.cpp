#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>

// Prunes the search space considerably.
static std::set<std::string> g_visited{};

auto identify(
    std::map<std::string_view, std::vector<std::set<std::string_view>>> allergies_to_foods,
    std::map<std::string_view, std::string_view> determined_allergen,
    std::set<std::string_view> remaining_allergens,
    std::set<std::string_view> used_allergens
) -> void
{
    // Once all allergens have been slotted we know we have a solution.
    if(remaining_allergens.empty())
    {
        // Inverting is necessary since they need to be alphabetically ordered by their allergens.
        std::map<std::string_view, std::string_view> inverted{};

        std::cout << "Allergens:\n";
        for(const auto& [i, a] : determined_allergen)
        {
            std::cout << i << " = " << a << "\n";
            inverted.emplace(a, i);
        }
        std::cout << "\n";

        std::cout << "Canonical dangerous ingredient list?\n";
        std::cout << chain::str::map_join(
            inverted,
            ',',
            [](const auto& p) {
                const auto& [a, i] = p;
                return i;
            }
        );

        std::cout << "\n";
        std::exit(EXIT_SUCCESS);
    }
    else
    {
        if(!g_visited.emplace(chain::str::join(remaining_allergens, ',')).second)
        {
            return;
        }

        for(const auto& allergen : remaining_allergens)
        {
            auto& foods = allergies_to_foods[allergen];

            for(auto& ingredients : foods)
            {
                for(auto& ingredient : ingredients)
                {
                    bool common{true};
                    for(auto& i_check : foods)
                    {
                        if(std::find(i_check.begin(), i_check.end(), ingredient) == i_check.end())
                        {
                            common = false;
                            break;
                        }
                    }

                    // If a common ingredient has been found amongst the foods that contain this
                    // allergen then remove it and try and find the next ingredient.
                    if(common)
                    {
                        auto a2f = allergies_to_foods;
                        auto da = determined_allergen;
                        auto ra = remaining_allergens;
                        auto ua = used_allergens;

                        da[ingredient] = allergen;
                        ra.erase(allergen);
                        ua.insert(allergen);

                        for(auto& [a, fs] : a2f)
                        {
                            for(auto& f : fs)
                            {
                                f.erase(ingredient);
                            }
                        }

                        identify(std::move(a2f), std::move(da), std::move(ra), std::move(ua));
                    }
                }
            }
        }
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

    std::map<std::string_view, std::vector<std::set<std::string_view>>> allergies_to_foods{};
    std::set<std::string_view> all_allergens{};

    for(auto& line : chain::str::split(contents, '\n'))
    {
        auto all_parts = chain::str::split(line, " (contains ");
        all_parts[1].remove_suffix(1); // strip trailing ')'

        auto ingredients_parts = chain::str::split(all_parts[0], ' ');
        auto allergens_parts = chain::str::split(all_parts[1], ", ");

        std::set<std::string_view> i_set(ingredients_parts.begin(), ingredients_parts.end());

        for(const auto& a : allergens_parts)
        {
            allergies_to_foods[a].emplace_back(i_set);
            all_allergens.emplace(a);
        }
    }

    std::map<std::string_view, std::string_view> determined_allergens{};
    std::set<std::string_view> used_allergens;

    identify(allergies_to_foods, determined_allergens, all_allergens, used_allergens);

    return 0;
}
