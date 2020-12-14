#include <lib/FileUtil.h>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <list>
#include <deque>

enum class equipment_type
{
    generator,
    microchip
};

auto to_string(equipment_type type) -> char
{
    switch(type)
    {
        case equipment_type::generator:
            return 'G';
        case equipment_type::microchip:
            return 'M';
    }
    throw std::runtime_error("unknown equipment_type" + std::to_string(static_cast<int64_t>(type)));
}

struct equipment
{
    char material;
    equipment_type type;

    auto operator==(const equipment& other) const -> bool
    {
        return material == other.material && type == other.type;
    }

    auto operator!=(const equipment& other) const -> bool
    {
        return !(*this == other);
    }

    auto operator<(const equipment& other) const -> bool
    {
        if(material < other.material)
        {
            return true;
        }
        if(material == other.material)
        {
            if(type < other.type)
            {
                return true;
            }
        }
        return false;
    }

    friend auto operator<<(std::ostream& os, const equipment& e) -> std::ostream&
    {
        os << e.material << ::to_string(e.type);
        return os;
    }

    auto to_string() const -> std::string
    {
        std::string output;
        output += material;
        output += ::to_string(type);
        return output;
    }
};

struct floor
{
    auto operator==(const floor&) const -> bool = default;

    std::set<equipment> contents{};
};

enum class direction
{
    up,
    down
};

class building
{
public:
    auto elevator_floor() const -> size_t { return m_elevator_floor; }
    auto move(direction d, const std::vector<equipment>& equipements) const -> building
    {
        auto next = *this;

        size_t from = m_elevator_floor;
        size_t to{0};
        switch(d)
        {
            case direction::up:
            {
                if(elevator_floor() == 4)
                {
                    throw std::logic_error{"cannot move up past floor 4"};
                }
                next.m_elevator_floor++;
                to = next.m_elevator_floor;
            }
                break;
            case direction::down:
            {
                if(elevator_floor() == 1)
                {
                    throw std::logic_error{"cannot move down past floor 1"};
                }
                next.m_elevator_floor--;
                to = next.m_elevator_floor;
            }
                break;
        }

        for(const auto& e : equipements)
        {
            next.m_floors[from - 1].contents.erase(e);
            next.m_floors[to - 1].contents.insert(e);
        }
        return next;
    }

    auto floor(size_t i) const -> const ::floor& { return m_floors[i - 1]; }
    auto floor(size_t i) -> ::floor& { return m_floors[i - 1]; }

    auto material_column(char m) const -> size_t { return m_material_columns.find(m)->second; }
    auto material_column(char m, size_t c) -> void { m_material_columns[m] = c; }

    auto operator==(const building&) const -> bool = default;

    auto operator<(const building& other) const -> bool
    {
        // The trick here is that the pair of microchip + generator location can be swapped
        // for equivalence rather than _exact_ equality.  This allows us to prune an extremely
        // large search space (8m20s run time to 4s runtime).  Comparing the building to_string()
        // version works and is very correct, but it doesn't prune enough of the search space to
        // make the algorithm very efficient.

        // This generates pairs for the location of each equipment type, and then sorts them
        // from lowest to highest in a multiset, and then generates the less than from this ordered
        // set of the locations of each equipmen WITHOUT their material type.

        // Making these static thread_local shaves a full second off the runtime, stringstream
        // constructor is pretty expensive.
        thread_local static std::stringstream os1{};
        thread_local static std::stringstream os2{};

        os1.clear();
        os1.str("");

        os2.clear();
        os2.str("");

        os1 << m_elevator_floor;
        os2 << other.m_elevator_floor;

        struct pairs
        {
            size_t microchip_floor;
            size_t generator_floor;

            auto operator<(const pairs& other) const -> bool
            {
                if(microchip_floor < other.microchip_floor)
                {
                    return true;
                }
                if(microchip_floor == other.microchip_floor)
                {
                    if(generator_floor < other.generator_floor)
                    {
                        return true;
                    }
                }
                return false;
            }
        };

        std::map<char, pairs> pairs1{};
        std::map<char, pairs> pairs2{};

        std::multiset<pairs> ordered1{};
        std::multiset<pairs> ordered2{};

        for(size_t i = 1; i <= 4; ++i)
        {
            const auto& f1 = floor(i);
            const auto& f2 = other.floor(i);

            for(const auto& e : f1.contents)
            {
                switch(e.type)
                {
                    case equipment_type::microchip:
                        pairs1[e.material].microchip_floor = i;
                        break;
                    case equipment_type::generator:
                        pairs1[e.material].generator_floor = i;
                        break;
                }
            }

            for(const auto& e : f2.contents)
            {
                switch(e.type)
                {
                    case equipment_type::microchip:
                        pairs2[e.material].microchip_floor = i;
                        break;
                    case equipment_type::generator:
                        pairs2[e.material].generator_floor = i;
                        break;
                }
            }
        }

        for(const auto& [m, p] : pairs1)
        {
            ordered1.emplace(p);
        }

        for(const auto& [m, p] : pairs2)
        {
            ordered2.emplace(p);
        }

        for(const auto& p : ordered1)
        {
            os1 << "-" << p.microchip_floor << "-" << p.generator_floor;
        }

        for(const auto& p : ordered2)
        {
            os2 << "-" << p.microchip_floor << "-" << p.generator_floor;
        }

        return os1.str() < os2.str();
    }

    friend auto operator<<(std::ostream& os, const building& b) -> std::ostream&
    {
        for(int64_t i = 4; i >= 1; --i)
        {
            const auto& current_floor = b.floor(i);

            os << "F" << std::to_string(i) << " ";
            if(b.elevator_floor() == i)
            {
                os << "E ";
            }
            else
            {
                os << ". ";
            }

            std::vector<std::string> output_columns{};
            output_columns.resize(b.m_material_columns.size() * 2);
            for(auto& col_data : output_columns)
            {
                col_data = ".  ";
            }

            for(const auto& e : current_floor.contents)
            {
                auto col = b.material_column(e.material);
                if(e.type == equipment_type::microchip)
                {
                    col++;
                }
                output_columns[col] = e.to_string() + " ";
            }

            for(const auto& col_data : output_columns)
            {
                os << col_data;
            }

            os << "\n";
        }

        return os;
    }

private:
    size_t m_elevator_floor{1};
    std::array<::floor, 4> m_floors{};
    static std::map<char, size_t> m_material_columns;
};

std::map<char, size_t> building::m_material_columns{};

static inline auto visited(const building& b, std::set<building>& seen) -> bool
{
    return !seen.emplace(b).second;
}

static auto find_shortest_path_bfs(
    building start,
    size_t equipment_count
) -> size_t
{
    // This is the set of all seen pair configurations of each equipment, this does not take the
    // material into account since material pairs can be interchanged with the same number of
    // steps to the end goal.
    std::set<building> seen{};

    // This is the current search space for the breadth first search.
    std::deque<std::list<building>> possibilities{};

    possibilities.emplace_back(std::list<building>{start});

    size_t min_steps = std::numeric_limits<size_t>::max();

    while(!possibilities.empty())
    {
        auto steps = std::move(possibilities.front());
        possibilities.pop_front();

        if(steps.size() > min_steps)
        {
            continue;
        }

        auto& current_building = steps.back();

        auto elevator_at = current_building.elevator_floor();
        auto& floor = current_building.floor(elevator_at);

        if(elevator_at == 4 && floor.contents.size() == equipment_count)
        {
            std::cout << "----------start solution----------\n";
            std::cout << "found end state in " << (steps.size() - 1) << " steps\n";
            for(const auto& b : steps)
            {
                std::cout << b << "\n";
            }
            std::cout << "----------end solution----------\n";

            min_steps = std::min(min_steps, (steps.size() - 1));
        }
        else
        {
            // If we've brought any chips to this floor and it doesn't have its corresponding generator
            // and there is another generator there, its now fried so this path doesn't work.
            bool has_generators{false};
            for(const auto& e : floor.contents)
            {
                if(e.type == equipment_type::generator)
                {
                    has_generators = true;
                    break;
                }
            }
            bool fried_chips{false};
            if(has_generators)
            {
                for(const auto& e : floor.contents)
                {
                    if(e.type == equipment_type::microchip)
                    {
                        if(floor.contents.find(equipment{e.material, equipment_type::generator}) == floor.contents.end())
                        {
                            fried_chips = true;
                            break;
                        }
                    }
                }
            }
            if(fried_chips)
            {
                continue;
            }

            // 1) Move generator with microchip upwards.
            // 2) Move single microchip upwards.
            // 3) Move single microchip downwards.
            // 4) Move two microchips upwards.
            // 5) Move two microchips downwards.

            // 6) Move 1 generator upwards.
            // 7) Move 1 generator downwards.
            // 8) Move two generators upwards.
            // 9) Move two generators downwards.

            // 1) Move generator with microchip upwards.
            for(const auto& e : floor.contents)
            {
                if(e.type == equipment_type::microchip)
                {
                    // If both items are on the floor move them up and recurse.
                    if(floor.contents.find(equipment{e.material, equipment_type::generator}) != floor.contents.end())
                    {
                        std::vector<equipment> to_move{
                            {e.material, equipment_type::generator},
                            {e.material, equipment_type::microchip}
                        };

                        if(elevator_at < 4)
                        {
                            auto next_building = current_building.move(direction::up, to_move);
                            if(!visited(next_building, seen))
                            {
                                steps.emplace_back(std::move(next_building));
                                possibilities.emplace_back(steps);
                                steps.pop_back();
                            }
                        }
                    }
                }
            }

            // 2) Move single microchip upwards
            // 3) Move single microchip downwards
            for(const auto& e : floor.contents)
            {
                if(e.type == equipment_type::microchip)
                {
                    std::vector<equipment> to_move{e};

                    if(elevator_at < 4)
                    {
                        auto next_building = current_building.move(direction::up, to_move);
                        if(!visited(next_building, seen))
                        {
                            steps.emplace_back(std::move(next_building));
                            possibilities.emplace_back(steps);
                            steps.pop_back();
                        }
                    }

                    if(elevator_at > 1)
                    {
                        auto next_building = current_building.move(direction::down, to_move);
                        if(!visited(next_building, seen))
                        {
                            steps.emplace_back(std::move(next_building));
                            possibilities.emplace_back(steps);
                            steps.pop_back();
                        }
                    }
                }
            }

            // 4) Move two microchips upwards.
            // 5) Move two microchips downwards.
            std::vector<equipment> microchips{};
            for(const auto& e : floor.contents)
            {
                if(e.type == equipment_type::microchip)
                {
                    microchips.emplace_back(e);
                }
            }

            // Already moved single microchips, can ignore when only 1 is found on this floor.
            if(microchips.size() > 1)
            {
                for(const auto& e1 : microchips)
                {
                    for(const auto& e2 : microchips)
                    {
                        if(e1 != e2)
                        {
                            std::vector<equipment> es{e1, e2};

                            if(elevator_at < 4)
                            {
                                auto next_building = current_building.move(direction::up, es);
                                if(!visited(next_building, seen))
                                {
                                    steps.emplace_back(std::move(next_building));
                                    possibilities.emplace_back(steps);
                                    steps.pop_back();
                                }
                            }

                            if(elevator_at > 1)
                            {
                                auto next_building = current_building.move(direction::down, es);
                                if(!visited(next_building, seen))
                                {
                                    steps.emplace_back(std::move(next_building));
                                    possibilities.emplace_back(steps);
                                    steps.pop_back();
                                }
                            }
                        }
                    }
                }
            }

            // 6) Move 1 generator upwards.
            // 7) Move 1 generator downwards.
            for(const auto& e : floor.contents)
            {
                if(e.type == equipment_type::generator)
                {
                    std::vector<equipment> to_move{e};

                    if(elevator_at < 4)
                    {
                        auto next_building = current_building.move(direction::up, to_move);
                        if(!visited(next_building, seen))
                        {
                            steps.emplace_back(std::move(next_building));
                            possibilities.emplace_back(steps);
                            steps.pop_back();
                        }
                    }

                    if(elevator_at > 1)
                    {
                        auto next_building = current_building.move(direction::down, to_move);
                        if(!visited(next_building, seen))
                        {
                            steps.emplace_back(std::move(next_building));
                            possibilities.emplace_back(steps);
                            steps.pop_back();
                        }
                    }
                }
            }

            // 8) Move two generators upwards.
            // 9) Move two generators downwards.
            std::vector<equipment> generators{};
            for(const auto& g : floor.contents)
            {
                if(g.type == equipment_type::generator)
                {
                    generators.emplace_back(g);
                }
            }

            if(generators.size() > 1)
            {
                for(const auto& g1 : generators)
                {
                    for(const auto& g2 : generators)
                    {
                        if(g1 != g2)
                        {
                            std::vector<equipment> gs{g1, g2};

                            if(elevator_at < 4)
                            {
                                auto next_building = current_building.move(direction::up, gs);
                                if(!visited(next_building, seen))
                                {
                                    steps.emplace_back(std::move(next_building));
                                    possibilities.emplace_back(steps);
                                    steps.pop_back();
                                }
                            }

                            if(elevator_at > 1)
                            {
                                auto next_building = current_building.move(direction::down, gs);
                                if(!visited(next_building, seen))
                                {
                                    steps.emplace_back(std::move(next_building));
                                    possibilities.emplace_back(steps);
                                    steps.pop_back();
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return min_steps;
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

    building b{};

    size_t current_column{0};
    size_t current_floor{1};
    for(auto& line : lines)
    {
        auto& floor = b.floor(current_floor);

        auto parts = chain::str::split(line, " floor contains ");
        auto contains = parts[1];
        contains.remove_suffix(1); // drop the '.'

        if(contains != "nothing relevant")
        {
            auto contains_parts = chain::str::split(contains, ", ");
            for(auto& part : contains_parts)
            {
                if(chain::str::starts_with(part, "and a "))
                {
                    part.remove_prefix(6);
                }
                else if(chain::str::starts_with(part, "a "))
                {
                    part.remove_prefix(2);
                }

                if(part.find("generator") != std::string_view::npos)
                {
                    auto generator_parts = chain::str::split(part, ' ');
                    auto material = std::toupper(generator_parts[0][0]);

                    floor.contents.emplace(material, equipment_type::generator);
                }
                else
                {
                    auto microchip_parts = chain::str::split(part, '-');
                    auto material = std::toupper(microchip_parts[0][0]);

                    floor.contents.emplace(material, equipment_type::microchip);

                    b.material_column(material, current_column);
                    current_column += 2;
                }
            }
        }

        ++current_floor;
    }

    auto min_steps = find_shortest_path_bfs(std::move(b), current_column);

    std::cout << "minimum number of steps is " << min_steps << "\n";

    return 0;
}
