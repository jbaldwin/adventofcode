#include <iostream>
#include <vector>
#include <string>
#include <optional>

#include <lib/FileUtil.h>
#include <lib/StringUtil.h>

struct Info
{
    int64_t id{0};
    std::optional<int64_t> children;
    std::optional<int64_t> cats;
    std::optional<int64_t> samoyeds;
    std::optional<int64_t> pomeranians;
    std::optional<int64_t> akitas;
    std::optional<int64_t> vizslas;
    std::optional<int64_t> goldfish;
    std::optional<int64_t> trees;
    std::optional<int64_t> cars;
    std::optional<int64_t> perfumes;

    auto operator[](std::size_t i) -> std::optional<int64_t>&
    {
        switch(i)
        {
            case 0: return children;
            case 1: return cats;
            case 2: return samoyeds;
            case 3: return pomeranians;
            case 4: return akitas;
            case 5: return vizslas;
            case 6: return goldfish;
            case 7: return trees;
            case 8: return cars;
            case 9: return perfumes;
            default:
                throw std::out_of_range("Info[" + std::to_string(i) + "] is out of range.");
        }
    }

    auto operator[](std::size_t i) const -> const std::optional<int64_t>&
    {
        switch(i)
        {
            case 0: return children;
            case 1: return cats;
            case 2: return samoyeds;
            case 3: return pomeranians;
            case 4: return akitas;
            case 5: return vizslas;
            case 6: return goldfish;
            case 7: return trees;
            case 8: return cars;
            case 9: return perfumes;
            default:
                throw std::out_of_range("Info[" + std::to_string(i) + "] is out of range.");
        }
    }

    friend auto operator<<(std::ostream& os, const Info& i) -> std::ostream&
    {
        std::cout << "{";
        std::cout << "id=" << i.id;
        std::cout << "[";
        for(auto j = 0; j < 10; ++j)
        {
            std::cout << i[j].value_or(-1) << ' ';
        }
        std::cout << "]";
        std::cout << "}";

        return os;
    }
};

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    Info match{};
    match.id = 0;
    match.children = 3;
    match.cats = 7;
    match.samoyeds = 2;
    match.pomeranians = 3;
    match.akitas = 0;
    match.vizslas = 0;
    match.goldfish = 5;
    match.trees = 3;
    match.cars = 2;
    match.perfumes = 1;

    auto contents = file::read(args[1]);
    auto lines = str::split(contents, '\n');

    std::vector<Info> aunts{};
    aunts.reserve(501);

    aunts.push_back(match);

    for(const auto& line : lines)
    {
        auto parts = str::split(line, ' ');

        Info info{};

        info.id = std::stol(std::string{parts[1]});
        
        for(std::size_t i = 2; i < parts.size(); i += 2)
        {
            auto type = parts[i];
            auto amount = std::stol(std::string{parts[i + 1]});

            if(type == "children:")
            {
                info.children = amount;
            }
            else if(type == "cats:")
            {
                info.cats = amount;
            }
            else if(type == "samoyeds:")
            {
                info.samoyeds = amount;
            }
            else if(type == "pomeranians:")
            {
                info.pomeranians = amount;
            }
            else if(type == "akitas:")
            {
                info.akitas = amount;
            }
            else if(type == "vizslas:")
            {
                info.vizslas = amount;
            }
            else if(type == "goldfish:")
            {
                info.goldfish = amount;
            }
            else if(type == "trees:")
            {
                info.trees = amount;
            }
            else if(type == "cars:")
            {
                info.cars = amount;
            }
            else if(type == "perfumes:")
            {
                info.perfumes = amount;
            }
            else
            {
                std::cerr << "Unknown compound " << type << '\n';
            }
        }

        aunts.push_back(info);
    }

    std::vector<Info> swap{};
    for(std::size_t i = 0; i < 10; ++i)
    {
        swap.clear();

        swap.push_back(aunts[0]); // always copy 0 over

        for(std::size_t j = 1; j < aunts.size(); ++j)
        {
            if(aunts[j][i].has_value())
            {
                auto value = aunts[j][i].value();

                // cats or trees value is >
                if(i == 1 || i == 7)
                {
                    if(value > aunts[0][i].value())
                    {
                        swap.push_back(aunts[j]);
                    }
                }
                else if(i == 3 || i == 6)
                {
                    if(value < aunts[0][i].value())
                    {
                        swap.push_back(aunts[j]);
                    }
                }
                else if(aunts[0][i].value() == value)
                {
                    swap.push_back(aunts[j]);
                }
            }
            else
            {
                swap.push_back(aunts[j]);
            }
        }

        aunts.swap(swap);

        std::cout << "i=" << i << '\n';
        for(const auto& aunt : aunts)
        {
            std::cout << aunt << '\n';
        }

        std::cout << "\n\n";
    }

    for(const auto& aunt : aunts)
    {
        std::cout << aunt << '\n';
    }

    return 0;
}
