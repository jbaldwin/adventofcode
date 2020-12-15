#include <iostream>
#include <vector>
#include <map>
#include <array>
#include <cmath>

#include <lib/AStar.hpp>
#include <lib/file_util.hpp>

static std::array<std::pair<int64_t, int64_t>, 4> movements = {
    std::pair{1, 0},
    std::pair{-1, 0}, 
    std::pair{0, 1}, 
    std::pair{0, -1}
};

struct Node;
using Map = std::map<int64_t, std::map<int64_t, std::unique_ptr<Node>>>;

auto copy_map(
    Map& orig,
    uint64_t level
) -> Map;

enum class PortalSide
{
    OUTER,
    INNER
};

struct Portal
{
    std::string name{};
    int64_t to_x{-1};
    int64_t to_y{-1};
    PortalSide side{PortalSide::OUTER};
};

struct Node : public astar::INode
{
    Node(
        Map& _original_map,
        uint64_t _level,
        std::map<uint64_t, Map>& _maps,
        int64_t _x,
        int64_t _y,
        char _tile
    )
        : original_map(_original_map)
        , level(_level)
        , maps(_maps)
        , x(_x)
        , y(_y)
        , tile(_tile)
    {
        
    }

    uint64_t level{0};
    Map& original_map;
    std::map<uint64_t, Map>& maps;
    int64_t x;
    int64_t y;
    char tile;
    std::optional<Portal> portal;

    virtual auto MovementCost(INode& parent) -> void override
    {
        auto& p = static_cast<Node&>(parent);
        movement_cost = 1 + p.movement_cost;
    }

    virtual auto EstimatedCost(INode& goal) -> void
    {
        auto& g = static_cast<Node&>(goal);
        // Each level will multiply its estimated cost by that level amount
        // to discourage going deeper into the levels if at all possible.
        estimated_cost 
            = std::abs(g.x - x)
            + std::abs(g.y - y)
            + std::abs(static_cast<int64_t>(g.level) - static_cast<int64_t>(level));

        // if(portal.has_value())
        // {
        //     auto& p = portal.value();

        //     if(p.side == PortalSide::INNER)
        //     {
        //         estimated_cost *= std::pow(level, level);
        //     }
        //     else
        //     {
        //         estimated_cost *= level;
        //     }
        // }
    }

    virtual auto Children() -> std::vector<INode*>&
    {
        if(!children.has_value())
        {
            std::vector<INode*> c{};

            auto can_move = [&](int64_t x, int64_t y) -> bool {
                try
                {
                    // can move through empty spaces or portals.
                    auto& node = maps[level].at(y).at(x);
                    if(node->tile == '.')
                    {
                        return true;
                    }

                    if(node->portal.has_value())
                    {
                        const auto& portal = node->portal.value();
                        if(portal.to_x == -1 && portal.to_y == -1)
                        {
                            return false;
                        }

                        if(level == 0)
                        {
                            // Only ZZ and inner portals are available on level 0.
                            if(portal.name == "ZZ")
                            {
                                return true;
                            }

                            if(portal.side == PortalSide::INNER)
                            {
                                return true;
                            }

                            return false;
                        }
                        else
                        {
                            // AA and ZZ are not available on any other level.
                            if(portal.name == "AA" || portal.name == "ZZ")
                            {
                                return false;
                            }

                            return true;
                        }
                    }
                }
                catch(...) { }
                return false;
            };

            for(const auto& [x1, y1] : movements)
            {
                if(can_move(x + x1, y + y1))
                {
                    auto& n = maps[level][y + y1][x + x1];
                    if(!n->portal.has_value())
                    {
                        c.emplace_back(n.get());
                    }
                    else
                    {
                        const auto& portal = n->portal.value();

                        if(level == 0 && portal.name == "ZZ")
                        {
                            c.emplace_back(n.get());
                        }
                        else
                        {
                            if(portal.side == PortalSide::OUTER)
                            {
                                auto plevel = level - 1;

                                auto* node_ptr = maps[plevel][portal.to_y][portal.to_x].get();

                                if(node_ptr == nullptr)
                                {
                                    std::cerr 
                                        << "level " << plevel << " going up "
                                        << " has a nullptr node at portal to " 
                                        << portal.to_x << ' ' << portal.to_y << "\n";
                                }

                                c.emplace_back(node_ptr);
                            }
                            else
                            {
                                auto plevel = level + 1;

                                if(auto found = maps.find(plevel); found == maps.end())
                                {
                                    std::cout << "Creating level " << plevel 
                                        << "from " << portal.name << "\n";
                                    std::cout << "From [" << n->x << ',' << n->y << "] to [" 
                                        << portal.to_x << ',' << portal.to_y << "]\n";

                                    maps[plevel] = copy_map(original_map, plevel);
                                }

                                auto* node_ptr = maps[plevel][portal.to_y][portal.to_x].get();

                                if(node_ptr == nullptr)
                                {
                                    std::cerr 
                                        << "level " << plevel << " going down "
                                        << " has a nullptr node at portal to " 
                                        << portal.to_x << ' ' << portal.to_y << "\n";
                                }

                                c.emplace_back(node_ptr);
                            }
                        }
                    }
                }
            }

            children = std::move(c);
        }
        
        return children.value();
    }

    virtual auto IsGoal(INode& goal) const -> bool
    {
        auto& g = static_cast<Node&>(goal);
        return (level == 0 && x == g.x && y == g.y);
    }
};

auto print_map(
    Map& map
) -> void
{
    for(std::size_t y = 0; y < map.size(); ++y)
    {
        for(std::size_t x = 0; x < map[0].size(); ++x)
        {
            auto& node = map[y][x];
            std::cout << node->tile;
        }
        std::cout << "\n";
    }
}

auto copy_map(
    Map& orig,
    uint64_t level
) -> Map
{
    Map out{};

    for(std::size_t y = 0; y < orig.size(); ++y)
    {
        for(std::size_t x = 0; x < orig[0].size(); ++x)
        {
            const auto& node = orig[y][x];
            out[y][x] = std::make_unique<Node>(*orig[y][x].get());
            out[y][x]->level = level;
        }
    }

    return out;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::map<uint64_t, Map> maps{};
    Map map{};

    auto contents = file::read(args[1]);

    int64_t x{0};
    int64_t y{0};
    for(auto c : contents)
    {
        switch(c)
        {
            case '\n':
            {
                x = 0;
                ++y;
            }
            break;
            case '#':
            {
                map[y][x] = std::make_unique<Node>(map, 0, maps, x, y, c);
                ++x;
            }
            break;
            case '.':
            {
                map[y][x] = std::make_unique<Node>(map, 0, maps, x, y, c);
                ++x;
            }
            break;
            case ' ':
            {
                map[y][x] = std::make_unique<Node>(map, 0, maps, x, y, c);
                ++x;
            }
            break;
            default:
            {
                map[y][x] = std::make_unique<Node>(map, 0, maps, x, y, c);
                ++x;
            }
            break;
        }
    }

    int64_t start_x{0};
    int64_t start_y{0};

    int64_t stop_x{0};
    int64_t stop_y{0};


    for(std::size_t y = 0; y < map.size(); ++y)
    {
        for(std::size_t x = 0; x < map[0].size(); ++x)
        {
            auto& node = map[y][x];

            auto is_portal_pair = [&](int64_t x1, int64_t y1) -> bool {
                try
                {
                    auto& pair = map.at(y1).at(x1);
                    if(std::isupper(pair->tile))
                    {
                        return true;
                    }
                }
                catch(...) {}
                return false;
            };

            auto is_next_to_space = [&](int64_t x1, int64_t y1) -> bool {
                try
                {
                    auto& node = map.at(y1).at(x1);
                    if(node->tile == '.')
                    {
                        return true;
                    }
                } catch(...) {}
                return false;
            };

            std::array<std::pair<int64_t, int64_t>, 2> name_moves = {
                std::pair{1, 0},
                std::pair{0, 1}
            };

            if(std::isupper(node->tile))
            {
                for(const auto& [x1, y1] : name_moves)
                {
                    if(is_portal_pair(x + x1, y + y1))
                    {
                        auto& node1 = map[y + y1][x + x1];

                        std::string name;
                        name += node->tile;
                        name += node1->tile;

                        bool found_space{false};
                        for(const auto& [x2, y2] : movements)
                        {
                            if(is_next_to_space(node->x + x2, node->y + y2))
                            {
                                found_space = true;

                                Portal portal;

                                portal.name = std::move(name);

                                if(portal.name == "AA")
                                {
                                    start_x = node->x + x2;
                                    start_y = node->y + y2;
                                }

                                if(portal.name == "ZZ")
                                {
                                    stop_x = node->x + x2;
                                    stop_y = node->y + y2;
                                }

                                std::cout << "Found portal: " << portal.name;
                                std::cout << "[" << node->x << ',' << node->y << "]\n";

                                node->portal = std::move(portal);

                                break;
                            }
                        }

                        if(!found_space)
                        {
                            for(const auto& [x2, y2] : movements)
                            {
                                if(is_next_to_space(node1->x + x2, node1->y + y2))
                                {
                                    Portal portal;

                                    portal.name = std::move(name);

                                    if(portal.name == "AA")
                                    {
                                        start_x = node1->x + x2;
                                        start_y = node1->y + y2;
                                    }

                                    if(portal.name == "ZZ")
                                    {
                                        stop_x = node1->x + x2;
                                        stop_y = node1->y + y2;
                                    }

                                    std::cout << "Found portal: " << portal.name;
                                    std::cout << "[" << node1->x << ',' << node1->y << "]\n";

                                    node1->portal = std::move(portal);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    std::cout << "Start position = [" << start_x << ',' << start_y << "]\n";
    std::cout << "Stop position = [" << stop_x << ',' << stop_y << "]\n";

    for(std::size_t y = 0; y < map.size(); ++y)
    {
        for(std::size_t x = 0; x < map[0].size(); ++x)
        {
            auto& node = map[y][x];
            if(node->portal.has_value())
            {
                auto& portal = node->portal.value();
                if(portal.to_x == -1 && portal.to_y == -1)
                {
                    for(std::size_t y1 = 0; y1 < map.size(); ++y1)
                    {
                        for(std::size_t x1 = 0; x1 < map[0].size(); ++x1)
                        {
                            // don't link ourself!
                            if(x == x1 && y == y1)
                            {
                                continue;
                            }

                            auto& node1 = map[y1][x1];
                            if(node1->portal.has_value())
                            {
                                auto& portal1 = node1->portal.value();
                                if(portal1.to_x == -1 && portal1.to_y == -1)
                                {
                                    if(portal.name == portal1.name)
                                    {
                                        std::cout << "Linking portals: " << portal.name;
                                        std::cout << "[" << node->x << ',' << node->y << "] [" << node1->x << ',' << node1->y << "]\n";
                                        
                                        portal.to_x = x1;
                                        portal.to_y = y1;

                                        portal1.to_x = x;
                                        portal1.to_y = y;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for(std::size_t y = 0; y < map.size(); ++y)
    {
        for(std::size_t x = 0; x < map[0].size(); ++x)
        {
            auto& node = map[y][x];
            if(node->portal.has_value())
            {
                if(
                    (node->x <= 1 || node->x + 2 >= map[0].size())
                    ||
                    (node->y <= 1 || node->y + 2 >= map.size())
                )
                {
                    std::cout << node->portal.value().name << "[" << node->x << "," << node->y << "] outer\n";
                    node->portal.value().side = PortalSide::OUTER;
                }
                else
                {
                    std::cout << node->portal.value().name << "[" << node->x << "," << node->y << "] inner\n";
                    node->portal.value().side = PortalSide::INNER;
                }
                
            }
        }
    }

    print_map(map);

    // Start with the first level.
    maps[0] = copy_map(map, 0);

    astar::AStar a{
        *maps[0][start_y][start_x].get(),
        *maps[0][stop_y][stop_x].get()
    };

    auto result = a.Run();
    switch(result)
    {
        case astar::State::FAILED:
            std::cerr << "AStar failed to find a path.\n";
        break;
        case astar::State::SEARCHING:
            std::cerr << "AStar.. is still searching..?\n";
        break;
        case astar::State::GOALFOUND:
            std::cout << "AStar found a path.\n";

            uint64_t steps{0};
            for(auto* node : a.Path())
            {
                auto* n = static_cast<Node*>(node);
                if(!n->portal.has_value())
                {
                    n->tile = 'x';
                    ++steps;
                }
                // else
                // {
                //     ++steps;
                // }
            }
            map[start_y][start_x]->tile = 'S';
            map[stop_y][stop_x]->tile = 'E';

            for(auto& [l, m] : maps)
            {
                std::cout << "Level = " << l << "\n";
                print_map(m);
                std::cout << "\n\n";
            }

            std::cout << "Steps = " << steps - 1<< "\n";

        break;
    }

    return 0;
}
