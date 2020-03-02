// #include <iostream>
// #include <vector>
// #include <string>
// #include <map>
// #include <unordered_set>
// #include <set>
// #include <queue>

// #include <lib/FileUtil.h>
// #include <lib/StringUtil.h>
// #include <lib/AStar.hpp>

// // auto manufacture(
// //     std::string start_molecule,
// //     std::string_view target_molecule,
// //     const std::multimap<std::string, std::string>& replacements
// // ) -> uint64_t
// // {
// //     std::queue<std::pair<std::string, uint64_t>> q{};
// //     q.emplace(std::move(start_molecule), 0);

// //     while(!q.empty())
// //     {
// //         auto& [molecule, steps] = q.front();

// //         if(molecule == target_molecule)
// //         {
// //             return steps;
// //         }

// //         for(const auto& [from, to] : replacements)
// //         {
// //             std::size_t pos{0};
// //             while(true)
// //             {
// //                 if(auto found = molecule.find(from, pos); found != std::string::npos)
// //                 {
// //                     auto generated = molecule;
// //                     generated.replace(found, from.length(), to);

// //                     q.emplace(std::move(generated), steps + 1);

// //                     pos = found + 1;
// //                 }
// //                 else
// //                 {
// //                     break;
// //                 }
// //             }
// //         }

// //         q.pop();
// //     }
// // }

// // auto manufacture_reverse(
// //     std::string start_molecule,
// //     const std::multimap<std::string, std::string>& replacements
// // ) -> int64_t
// // {
// //     std::set<std::string> seen{};
// //     std::queue<std::pair<std::string, uint64_t>> q{};
// //     q.emplace(std::move(start_molecule), 0);

// //     while(!q.empty())
// //     {
// //         auto& [molecule, steps] = q.front();

// //         std::cout << molecule << ' ' << steps << '\n';

// //         if(molecule == "e")
// //         {
// //             return steps;
// //         }

// //         if(molecule.empty()) // i don't think this should happen?
// //         {
// //             continue;
// //         }

// //         for(const auto& [to, from] : replacements)
// //         {
// //             std::size_t pos{0};
// //             while(true)
// //             {
// //                 if(auto found = molecule.find(from, pos); found != std::string::npos)
// //                 {
// //                     auto generated = molecule;
// //                     generated.replace(found, from.length(), to);
// //                     pos = found + 1;

// //                     if(auto found = seen.find(generated); found == seen.end())
// //                     {
// //                         seen.emplace(generated);
// //                         q.emplace(std::move(generated), steps + 1);
// //                     }
// //                 }
// //                 else
// //                 {
// //                     break;
// //                 }
// //             }
// //         }

// //         q.pop();
// //     }

// //     return 0;
// // }

// // auto manufacture_reverse(
// //     std::string molecule,
// //     uint64_t steps,
// //     const std::multimap<std::string, std::string>& replacements,
// //     std::set<std::string>& seen,
// //     std::map<std::string, uint64_t> paths
// // ) -> uint64_t
// // {
// //     std::cout << molecule << ' ' << steps << '\n';

// //     if(molecule == "e")
// //     {
// //         std::cout << "e found in " << steps << " steps\n";
// //         return steps;
// //     }

// //     uint64_t min_steps = std::numeric_limits<uint64_t>::max();
// //     if(molecule.empty())
// //     {
// //         std::cout << "found empty at " << steps << '\n';
// //         return min_steps;
// //     }

// //     // if(auto path_found = paths.find(molecule); path_found != paths.end())
// //     // {
// //     //     std::cout << "Path found " << path_found->second << '\n';
// //     //     return path_found->second;
// //     // }

// //     for(const auto& [to, from] : replacements)
// //     {
// //         std::size_t pos{0};
// //         while(true)
// //         {
// //             if(auto found = molecule.find(from, pos); found != std::string::npos)
// //             {
// //                 auto generated = molecule;
// //                 generated.replace(found, from.length(), to);

// //                 pos = found + from.length();
// //                 if(auto found = seen.find(generated); found != seen.end())
// //                 {
// //                     // std::cout << "Dupe path found " << found->second << '\n'; 
// //                     continue;
// //                 }
// //                 seen.emplace(generated);

// //                 auto s = manufacture_reverse(
// //                     std::move(generated),
// //                     steps + 1,
// //                     replacements,
// //                     seen,
// //                     paths
// //                 );

// //                 min_steps = std::min(
// //                     min_steps,
// //                     s
// //                 );
// //             }
// //             else
// //             {
// //                 break;
// //             }
// //         }
// //     }

// //     // paths.emplace(std::move(molecule), min_steps);

// //     return min_steps;
// // }

// class Node;

// namespace std
// {

// template<>
// struct hash<std::unique_ptr<Node>>
// {
//     auto operator()(const std::unique_ptr<Node>& node) const -> std::size_t;
// };

// } // namespace std

// class Node : public astar::INode
// {
// public:
//     Node(
//         astar::AStar* _astar,
//         std::string _molecule,
//         std::unordered_set<std::unique_ptr<Node>>& _nodes,
//         const std::vector<std::pair<std::string, std::string>>& _replacements
//     )
//         : astar(_astar)
//         , molecule(std::move(_molecule))
//         , nodes(_nodes)
//         , replacements(_replacements)
//     {

//     }

//     auto operator==(const Node& other) const -> bool
//     {
//         return molecule == other.molecule;
//     }

//     virtual auto MovementCost(INode& parent) -> void override
//     {
//         movement_cost = parent.MovementCost() + 1;
//     }

//     virtual auto EstimatedCost(INode& goal) -> void override
//     {
//         auto& g = static_cast<Node&>(goal);
//         estimated_cost = std::abs(
//             static_cast<int64_t>(molecule.length()) - static_cast<int64_t>(g.molecule.length()));
//     }

//     virtual auto Children() -> std::vector<astar::INode*>& override
//     {
//         if(!children.has_value())
//         {
//             auto temp_node = std::make_unique<Node>(astar, "", nodes, replacements);
//             std::vector<astar::INode*> c;

//             for(const auto& [to, from] : replacements)
//             {
//                 std::size_t pos{0};
//                 while(true)
//                 {
//                     if(auto found = molecule.find(from, pos); found != std::string::npos)
//                     {
//                         temp_node->molecule = molecule;
//                         temp_node->molecule.replace(found, from.length(), to);
//                         pos = found + from.length();

//                         // bool already_checked{false};
//                         // for(const auto& [cost, inode] : astar->ClosedList())
//                         // {
//                         //     auto& node = static_cast<Node&>(inode);
//                         //     if(node.molecule == temp_node->molecule)
//                         //     {
//                         //         // std::cout << "Child is closed list already " << node.molecule << "\n";
//                         //         already_checked = true;
//                         //         break;
//                         //     }
//                         // }
//                         // if(already_checked) continue;

//                         bool node_exists{false};
//                         for(auto& node_ptr : nodes)
//                         {
//                             if(node_ptr->molecule == temp_node->molecule)
//                             {
//                                 auto* child = node_ptr.get();
//                                 // std::cout << child->molecule << " exists already\n";
//                                 // if(child->OpenList() || child->ClosedList())
//                                 // {
//                                 //     std::cout << "Child is open/closed list already\n";
//                                 // }
//                                 c.emplace_back(child);
//                                 node_exists = true;
//                                 break;
//                             }
//                         }
//                         // if(auto exists = nodes.find(temp_node); exists != nodes.end())
//                         {
                            
//                         }
//                         if(!node_exists)
//                         {
//                             auto [iter, inserted] = nodes.emplace(
//                                 std::make_unique<Node>(
//                                     astar,
//                                     temp_node->molecule,
//                                     nodes,
//                                     replacements
//                                 )
//                             );

//                             c.emplace_back((*iter).get());
//                         }
//                     }
//                     else
//                     {
//                         break;
//                     }
//                 }
//             }

//             children = std::move(c);
//         }

//         return children.value();
//     }

//     virtual auto IsGoal(INode& goal) const -> bool override
//     {
//         auto& g = static_cast<Node&>(goal);
//         return (molecule == g.molecule);
//     }

//     virtual auto Visited() -> void override
//     {
//         std::cout << "Visiting " << molecule << "\n";
//     }

//     astar::AStar* astar;
//     std::string molecule{};
//     std::unordered_set<std::unique_ptr<Node>>& nodes;
//     const std::vector<std::pair<std::string, std::string>>& replacements;
// };

// namespace std
// {

// // template<>
// auto hash<std::unique_ptr<Node>>::operator()(const std::unique_ptr<Node>& node) const -> std::size_t
// {
//     return std::hash<std::string>{}(node->molecule);
// }

// } // namespace std

// int main(int argc, char* argv[])
// {
//     std::vector<std::string> args{argv, argv + argc};
//     if(args.size() != 2)
//     {
//         std::cout << args[0] << " <input_file>" << std::endl;
//         return 1;
//     }

//     auto contents = file::read(args[1]);

//     auto parts = str::split(contents, "\n\n");
//     auto replacement_inputs_all = parts[0];
//     auto finish_molecule = std::string{parts[1]};

//     std::vector<std::pair<std::string, std::string>> replacements{};

//     for(const auto& line : str::split(replacement_inputs_all, '\n'))
//     {
//         auto replacement_parts = str::split(line, " => ");
//         replacements.emplace_back(
//             std::string{replacement_parts[0]},
//             std::string{replacement_parts[1]}
//         );
//     }

//     // std::sort(
//     //     replacements.begin(),
//     //     replacements.end(),
//     //     [](
//     //         const std::pair<std::string, std::string>& a,
//     //         const std::pair<std::string, std::string>& b
//     //     ) -> bool
//     //     {
//     //         return a.second.length() > b.second.length();
//     //     }
//     // );

//     for(const auto& [from, to] : replacements)
//     {
//         std::cout << from << " => " << to << '\n';
//     }

//     std::unordered_set<std::unique_ptr<Node>> nodes{};
    
//     auto start_ptr = std::make_unique<Node>(nullptr, finish_molecule, nodes, replacements);
//     auto goal_ptr = std::make_unique<Node>(nullptr, "e", nodes, replacements);

//     astar::AStar a{
//         *start_ptr.get(),
//         *goal_ptr.get()
//     };
    
//     start_ptr->astar = &a;
//     goal_ptr->astar = &a;

//     nodes.emplace(std::move(start_ptr));
//     nodes.emplace(std::move(goal_ptr));

//     auto state = a.Run();
//     switch(state)
//     {
//         case astar::State::FAILED:
//             std::cout << "AStar failed to find a path.\n";
//         break;
//         case astar::State::GOALFOUND:
//             std::cout << "AStar found the goal in " << (a.Path().size() - 1) << " steps.\n";
//         break;
//         case astar::State::SEARCHING:
//             std::cout << "AStar is still searching...\n";
//         break;
//     }

//     // std::set<std::string> seen{};
//     // std::map<std::string, uint64_t> paths{};
//     // auto steps = manufacture_reverse(
//     //     std::string{target_molecule}, 
//     //     0, 
//     //     replacements, 
//     //     seen,
//     //     paths
//     // );
//     // auto steps = manufacture_reverse(std::string{target_molecule}, replacements);

//     // std::cout << "Steps " << steps << '\n';

//     return 0;
// }

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <list>

#include <lib/FileUtil.h>
#include <lib/StringUtil.h>

struct Node
{
    Node(
        std::string _molecule,
        int64_t _cost,
        int64_t _score
    )
        : molecule(std::move(_molecule))
        , cost(_cost)
        , score(_score)
    {
        
    }

    std::string molecule;
    int64_t cost;
    int64_t score;

    auto operator<(const Node& other) const -> bool
    {
        if(molecule < other.molecule) return true;
        if(molecule == other.molecule)
        {
            return false;
        }
        return score < other.score;
    }
};

auto heuristic(const std::string& a, const std::string& b) -> int64_t
{
    return (a.length() - b.length());
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

    auto parts = str::split(contents, "\n\n");
    auto replacement_inputs_all = parts[0];
    auto finish_molecule = std::string{parts[1]};

    std::vector<std::pair<std::string, std::string>> replacements{};

    for(const auto& line : str::split(replacement_inputs_all, '\n'))
    {
        auto replacement_parts = str::split(line, " => ");
        replacements.emplace_back(
            std::string{replacement_parts[0]},
            std::string{replacement_parts[1]}
        );
    }

    for(const auto& [from, to] : replacements)
    {
        std::cout << from << " => " << to << '\n';
    }

    std::multimap<int64_t, Node> open_list{};
    std::set<std::string> open_molecules{};
    std::set<std::string> closed_list{};

    open_list.emplace(
        heuristic(finish_molecule, "e"),
        Node{finish_molecule, 0, heuristic(finish_molecule, "e")}
    );
    open_molecules.emplace(finish_molecule + std::to_string(heuristic(finish_molecule, "e")));

    int total = 0;

    while(!open_list.empty())
    {
        const auto& [c_score, current] = *open_list.begin();
        std::cout << current.molecule << " " << current.cost << " " << current.score << "\n";

        if(current.molecule == "e")
        {
            std::cout << current.cost << "\n";
            return 0;
        }

        auto current_key = current.molecule + std::to_string(c_score);
        closed_list.insert(current_key);

        for(const auto& [rep, repee] : replacements)
        {
            std::size_t offset = 0;
            while((offset = current.molecule.find(repee, offset)) != std::string::npos)
            {
                int length = repee.length();
                std::string generated = current.molecule;
                generated.replace(offset, repee.length(), rep);
                offset += length;

                auto ncost = current.cost + 1;
                auto score = ncost + heuristic(generated, "e");
                auto key = generated + std::to_string(score);
                
                if(auto found = closed_list.find(key); found != closed_list.end())
                {
                    continue;
                }

                Node neighbor{std::move(generated), ncost, score};

                if(auto found = open_molecules.find(key); found == open_molecules.end())
                {
                    open_molecules.insert(key);
                    open_list.emplace(neighbor.score, neighbor);
                }
            }
        }

        open_molecules.erase(current.molecule + std::to_string(c_score));
        open_list.erase(open_list.begin());
    }

    return 0;
}