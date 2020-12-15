#include <iostream>
#include <vector>
#include <map>
#include <array>

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

struct Node : public astar::INode
{
    Node(
        Map& _map,
        int64_t _x,
        int64_t _y,
        char _tile
    )
        : map(_map)
        , x(_x)
        , y(_y)
        , tile(_tile)
    {

    }

    Map& map;
    int64_t x;
    int64_t y;
    char tile;
    std::optional<std::string> portal_name;
    Node* portal_to{nullptr};

    virtual auto MovementCost(INode& parent) -> void override
    {
        auto& p = static_cast<Node&>(parent);
        movement_cost = 1 + p.movement_cost;
    }

    virtual auto EstimatedCost(INode& goal) -> void
    {
        auto& g = static_cast<Node&>(goal);
        estimated_cost = std::abs(g.x - x) + std::abs(g.y - y);
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
                    auto& node = map.at(y).at(x);
                    if(node->tile == '.')
                    {
                        return true;
                    }

                    if(node->portal_to != nullptr)
                    {
                        return true;
                    }
                }
                catch(...) { }
                return false;
            };

            for(const auto& [x1, y1] : movements)
            {
                if(can_move(x + x1, y + y1))
                {
                    auto& n = map[y + y1][x + x1];
                    if(n->portal_to == nullptr)
                    {
                        c.emplace_back(map[y + y1][x + x1].get());
                    }
                    else
                    {
                        // jump through the portal woooo!
                        c.emplace_back(n->portal_to);
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
        return (x == g.x && y == g.y);
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

int main(int argc, char* argv[])
{
    std::vector<std::string> args{argv, argv + argc};
    if(args.size() != 2)
    {
        std::cout << args[0] << " <input_file>" << std::endl;
        return 1;
    }

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
                map[y][x] = std::make_unique<Node>(map, x, y, c);
                ++x;
            }
            break;
            case '.':
            {
                map[y][x] = std::make_unique<Node>(map, x, y, c);
                ++x;
            }
            break;
            case ' ':
            {
                map[y][x] = std::make_unique<Node>(map, x, y, c);
                ++x;
            }
            break;
            default:
            {
                map[y][x] = std::make_unique<Node>(map, x, y, c);
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

                                node->portal_name = std::move(name);

                                if(node->portal_name.value() == "AA")
                                {
                                    start_x = node->x + x2;
                                    start_y = node->y + y2;
                                }

                                if(node->portal_name.value() == "ZZ")
                                {
                                    stop_x = node->x + x2;
                                    stop_y = node->y + y2;
                                }

                                std::cout << "Found portal: " << node->portal_name.value();
                                std::cout << "[" << node->x << ',' << node->y << "]\n";

                                break;
                            }
                        }

                        if(!found_space)
                        {
                            for(const auto& [x2, y2] : movements)
                            {
                                if(is_next_to_space(node1->x + x2, node1->y + y2))
                                {
                                    node1->portal_name = std::move(name);

                                    if(node1->portal_name.value() == "AA")
                                    {
                                        start_x = node1->x + x2;
                                        start_y = node1->y + y2;
                                    }

                                    if(node1->portal_name.value() == "ZZ")
                                    {
                                        stop_x = node1->x + x2;
                                        stop_y = node1->y + y2;
                                    }

                                    std::cout << "Found portal: " << node1->portal_name.value();
                                    std::cout << "[" << node1->x << ',' << node1->y << "]\n";
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
            if(node->portal_name.has_value() && node->portal_to == nullptr)
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
                        if(node1->portal_name.has_value() && node1->portal_to == nullptr)
                        {
                            if(node->portal_name.value() == node1->portal_name.value())
                            {
                                std::cout << "Linking portals: " << node->portal_name.value();
                                std::cout << "[" << node->x << ',' << node->y << "] [" << node1->x << ',' << node1->y << "]\n";
                                node->portal_to = node1.get();
                                node1->portal_to = node.get();
                            }
                        }
                    }
                }
            }
        }
    }

    print_map(map);

    astar::AStar a{
        *map[start_y][start_x].get(),
        *map[stop_y][stop_x].get()
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
                if(n->portal_to == nullptr)
                {
                    n->tile = 'x';
                    ++steps;
                }
            }
            map[start_y][start_x]->tile = 'S';
            map[stop_y][stop_x]->tile = 'E';

            print_map(map);

            std::cout << "Steps = " << steps - 1<< "\n";

        break;
    }

    return 0;
}
