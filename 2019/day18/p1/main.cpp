#include <iostream>
#include <vector>
#include <string_view>
#include <set>
#include <queue>
#include <variant>
#include <list>

#include <lib/AStar.hpp>
#include <lib/file_util.hpp>

struct Point
{
    int64_t x{0};
    int64_t y{0};

    friend auto operator<<(std::ostream& os, const Point& p) -> std::ostream&
    {
        os << '[' << p.x << ',' << p.y << ']';
        return os;
    }
};

struct Node
{
    char content{';'};
    bool visited{false};
};

using Map = std::vector<std::vector<Node>>;

auto print_map(
    const Map& map
) -> void
{
    for(const auto& row : map)
    {
        for(const auto& node : row)
        {
            std::cout << node.content;
        }
        std::cout << '\n';
    }
}

auto unlock_door(
    Map& map,
    char key
) -> void
{
    key = std::tolower(key);

    for(auto& row : map)
    {
        for(auto& node : row)
        {
            // turn the key and door into empty space
            if(std::tolower(node.content) == key)
            {
                node.content = '.';
            }

            node.visited = false;
        }
    }
}

auto map_count_keys(
    const Map& map
) -> int64_t
{
    int64_t key_count{0};
    for(const auto& row : map)
    {
        for(const auto& node : row)
        {
            if(std::islower(node.content))
            {
                ++key_count;
            }
        }
    }
    return key_count;
}

auto explore_bfs(
    Map map,
    Point start,
    std::set<int64_t>& paths,
    int64_t keys_remaining
) -> void
{
    std::list<Map> maps{};
    maps.emplace_back(std::move(map));

    std::deque<std::tuple<Map*, Point, int64_t, int64_t>> q{};
    q.emplace_back(&maps.back(), start, 0, keys_remaining);

    while(!q.empty())
    {
        auto& v = q.front();

        auto s = std::get<2>(v);

        if(!paths.empty() && s >= *paths.begin())
        {
            q.pop_front();
            continue;
        }

        auto& m = *std::get<0>(v);

        auto p = std::get<1>(v);
        auto k = std::get<3>(v);

        auto& node = m[p.y][p.x];
        node.visited = true;
        if(std::islower(node.content))
        {
            // found a key
            --k;

            if(k == 0)
            {
                std::cout << "Key was final key " << node.content << " with steps = " << s << std::endl;
                paths.insert(s);
            }
            else
            {
                auto copy = m;

                unlock_door(copy, node.content);

                maps.emplace_back(std::move(copy));
                // re-queue ourself until no keys remain
                q.emplace_back(&maps.back(), p, s, k);
            }
        }
        else
        {
            const std::array<Point, 4> neighbors 
            {
                Point{p.x + 1, p.y},
                Point{p.x - 1, p.y},
                Point{p.x, p.y + 1},
                Point{p.x, p.y - 1},
            };

            for(const auto& p_n : neighbors)
            {
                try
                {
                    auto& neighbor = m.at(p_n.y).at(p_n.x);
                    if(    !neighbor.visited
                        && neighbor.content != '#'
                        && !std::isupper(neighbor.content)
                    )
                    {
                        q.emplace_back(&m, p_n, s + 1, k);
                    }
                    // isn't a moveable tile, ignore
                }
                catch(...)
                {
                    // out of bounds, ignore
                }
            }
        }

        q.pop_front();
    }
}

auto explore_dfs(
    Map& map,
    Point pos,
    int64_t steps,
    std::set<int64_t>& paths,
    int64_t keys_remaining
) -> void
{
    try
    {
        if(!paths.empty() && steps >= *paths.begin())
        {
            return;
        }

        auto& node = map.at(pos.y).at(pos.x);
        // Skip all visited nodes.
        if(node.visited == true)
        {
            return;
        }

        node.visited = true;

        switch(node.content)
        {
            case '@':
            case '.':
            {
                auto can_move = [&](Point p) -> bool {
                    try
                    {
                        auto& neighbor = map.at(p.y).at(p.x);
                        if(neighbor.visited)
                        {
                            return false;
                        }

                        if(neighbor.content == '#')
                        {
                            return false;
                        }

                        if(std::isupper(neighbor.content))
                        {
                            return false;
                        }

                        return true;
                    }
                    catch(...)
                    {
                        return false;
                    }
                };

                if(can_move(Point{pos.x + 1, pos.y}))
                {
                    explore_dfs(map, Point{pos.x + 1, pos.y}, steps + 1, paths, keys_remaining);
                }
                if(can_move(Point{pos.x - 1, pos.y}))
                {
                    explore_dfs(map, Point{pos.x - 1, pos.y}, steps + 1, paths, keys_remaining);
                }
                if(can_move(Point{pos.x, pos.y + 1}))
                {
                    explore_dfs(map, Point{pos.x, pos.y + 1}, steps + 1, paths, keys_remaining);
                }
                if(can_move(Point{pos.x, pos.y - 1}))
                {
                    explore_dfs(map, Point{pos.x, pos.y - 1}, steps + 1, paths, keys_remaining);
                }
            }
            case '#':
                // skip all walls
                return;
            default:
            {
                if(std::islower(node.content))
                {
                    // this is a key!
                    //  std::cout << "Found key " << node.content << " [" << pos.x << ',' << pos.y << "] steps = " << steps << "\n";

                    --keys_remaining;
                    if(keys_remaining == 0)
                    {
                        // print_map(map);
                        std::cout << "Key was final key " << node.content << " with steps = " << steps << std::endl;
                        // std::cout << "\n";
                        paths.insert(steps);
                        return;
                    }

                    // Take a copy of the map so we don't interfere with
                    // other explorations that might not have found this key
                    // yet.  Then unlock the cooresponding door and explore
                    // on this node again (so its visited immediatly).
                    // Unlocking a door also marks every node as unvisited again.
                    auto copy = map;
                    unlock_door(copy, node.content);
                    explore_dfs(copy, pos, steps, paths, keys_remaining); // revisit ourself on the new map
                    return;
                }
                else
                {
                    // this is a door! ignore for now
                    return;
                }
            }
        }
    }
    catch(...)
    {
        return;
    }
}

struct Edge
{
    Point pos_a;
    Point pos_b;
    int64_t steps;
    char key_a;
    char key_b;
    std::set<char> doors;

    friend auto operator<<(std::ostream& os, const Edge& e) -> std::ostream&
    {
        os << "{";
        os << "pos_a=" << e.pos_a;
        os << ", pos_b=" << e.pos_b;
        os << ", steps=" << e.steps;
        os << ", key_a=" << e.key_a;
        os << ", key_b=" << e.key_b;
        os << ", doors=[";
        bool first{true};
        for(const auto& c : e.doors)
        {
            if(first)
            {
                first = false;
            }
            else
            {
                os << ",";
            }
            os << c;
        }
        os << "]}";
        return os;
    }
};

auto explore_a_to_b(
    Map map,
    Point pos_a,
    Point pos_b
) -> Edge
{
    std::queue<std::tuple<Point, int64_t, std::set<char>>> q{};
    q.emplace(pos_a, 0, std::set<char>{});

    std::map<int64_t, std::set<char>> paths{};

    Edge e{
        pos_a,
        pos_b,
        0,
        map[pos_a.y][pos_a.x].content,
        map[pos_b.y][pos_b.x].content,
        {}
    };

    while(!q.empty())
    {
        auto& item = q.front();

        auto& p = std::get<0>(item);
        auto  s = std::get<1>(item);

        if(!paths.empty())
        {
            auto smallest = *paths.begin();
            auto smallest_steps = std::get<0>(smallest);
            if(s >= smallest_steps)
            {
                q.pop();
                continue;
            }
        }

        auto& node = map[p.y][p.x];
        node.visited = true;

        auto& doors = std::get<2>(item);
        if(std::isupper(node.content))
        {
            doors.insert(node.content);
        }

        if(p.x == pos_b.x && p.y == pos_b.y)
        {
            // should union doors?
            paths.emplace(s, std::move(doors));
            q.pop();
            continue;
        }

        const std::array<Point, 4> neighbors 
        {
            Point{p.x + 1, p.y},
            Point{p.x - 1, p.y},
            Point{p.x, p.y + 1},
            Point{p.x, p.y - 1},
        };

        for(const auto& p_n : neighbors)
        {
            try
            {
                auto& neighbor = map.at(p_n.y).at(p_n.x);
                if(    !neighbor.visited
                    && neighbor.content != '#'
                )
                {
                    q.emplace(p_n, s + 1, doors);
                }
                // isn't a moveable tile, ignore
            }
            catch(...)
            {
                // out of bounds, ignore
            }
        }

        q.pop();
    }

    auto& [best_steps, best_doors] = *paths.begin();

    e.steps = best_steps;
    e.doors = best_doors;

    return e;
}

struct KeyDoor
{
    Point pos;
    char id;

    friend auto operator<<(std::ostream& os, const KeyDoor& kd) -> std::ostream&
    {
        os << "{pos=" << kd.pos << ",id=" << kd.id << "}";
        return os;
    }
};

auto explore_points(
    std::set<char>& keys,
    std::set<char>& open_doors,
    std::map<char, std::map<char, Edge>>& edges,
    std::set<int64_t>& paths,
    char current,
    std::string current_cache_key,
    int64_t steps,
    std::map<std::string, int64_t>& cache
) -> std::optional<int64_t>
{
    if(keys.empty())
    {
        if(paths.empty())
        {
            std::cout << "Found path with steps " << steps << "\n";
            paths.insert(steps);
        }
        else if(steps < *paths.begin())
        {
            std::cout << "Found path with steps " << steps << "\n";
            paths.insert(steps);
        }
        return {steps};
    }

    if(!paths.empty() && steps >= *paths.begin())
    {
        return {};
    }

    current_cache_key += current;
    for(const auto& k : keys)
    {
        current_cache_key += k;
    }
    
    if(auto found = cache.find(current_cache_key); found != cache.end())
    {
        paths.insert(steps + found->second);
        return {steps + found->second};
    }

    int64_t min = std::numeric_limits<int64_t>::max();

    for(const auto& [key_b, edge] : edges[current])
    {
        if(keys.find(key_b) != keys.end())
        {
            bool doors_open{true};
            for(const auto& req_door : edge.doors)
            {
                if(open_doors.find(req_door) == open_doors.end())
                {
                    doors_open = false;
                }
            }
            if(!doors_open)
            {
                continue;
            }

            open_doors.insert(std::toupper(edge.key_b));
            keys.erase(key_b);

            auto path_steps_opt = explore_points(
                keys,
                open_doors,
                edges,
                paths,
                key_b,
                current_cache_key,
                steps + edge.steps,
                cache
            );

            if(path_steps_opt.has_value())
            {
                min = std::min(min, path_steps_opt.value());
            }

            open_doors.erase(std::toupper(edge.key_b));
            keys.insert(key_b);
        }
    }

    cache.emplace(std::move(current_cache_key), min);

    return {min};
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

    Map map{};
    Point p{};
    Point start{};

    map.push_back({});
    auto* row = &map.back();

    std::vector<KeyDoor> keys{};
    std::vector<KeyDoor> doors{};

    while(!contents.empty())
    {
        auto c = contents[0];
        contents = contents.substr(1);

        switch(c)
        {
            case '\n':
                ++p.y;
                p.x = 0;
                map.push_back({});
                row = &map.back();
            break;
            case '@':
                row->push_back(Node{'@', false});
                start.x = p.x;
                start.y = p.y;
                ++p.x;
            break;
            case '#':
            case '.':
                row->push_back(Node{c, false});
                ++p.x;
            break;
            default:
            {
                if(std::isupper(c))
                {
                    doors.emplace_back(KeyDoor{p, c});
                }
                else
                {
                    keys.emplace_back(KeyDoor{p, c});
                }

                row->push_back(Node{c, false});
                ++p.x;
            }
            break;
        }
    }

    print_map(map);

    keys.emplace_back(KeyDoor{start, '@'});

    std::cout << "Keys:\n";
    for(const auto& key : keys)
    {
        std::cout << key << "\n";
    }
    std::cout << "\nDoors:\n";
    for(const auto& door : doors)
    {
        std::cout << door << "\n";
    }
    std::vector<Edge> edges{};
    for(const auto& key_a : keys)
    {
        for(const auto& key_b : keys)
        {
            if(key_a.id == key_b.id)
            {
                continue;
            }

            edges.emplace_back(explore_a_to_b(map, key_a.pos, key_b.pos));
        }
    }

    std::map<char, std::map<char, Edge>> map_edges{};
    for(const auto& e : edges)
    {
        map_edges[e.key_a][e.key_b] = e;
    }
    std::set<char> map_keys{};
    for(const auto& k : keys)
    {
        map_keys.insert(k.id);
    }

    std::cout << "\nEdges:\n";
    for(const auto& e : edges)
    {
        std::cout << e << "\n";
    }

    KeyDoor starting_point;
    for(std::size_t i = 0; i < keys.size(); ++i)
    {
        if(keys[i].id == '@')
        {
            starting_point = keys[i];
            keys.erase(keys.begin() + i);
            break;
        }
    }
    std::cout << "Selected starting point " << starting_point << "\n";

    std::map<std::string, int64_t> cache{};
    std::set<int64_t> paths{};
    std::set<char> open_doors{};
    explore_points(map_keys, open_doors, map_edges, paths, starting_point.id, "", 0, cache);

    std::cout << "\nSteps:\n";
    for(auto& steps : paths)
    {
        std::cout << steps << "\n";
    }

    // auto key_count = map_count_keys(map);

    // std::set<int64_t> paths{};
    // explore(map, start, 0, paths, key_count);
    // explore_bfs(map, start, paths, keys.size());

    // for(const auto& i : paths)
    // {
        // std::cout << i << "\n";
    // }

    return 0;
}
