#pragma once

#include <map>
#include <vector>
#include <memory>
#include <algorithm>
#include <optional>

namespace astar
{

enum class State
{
    SEARCHING,
    GOALFOUND,
    FAILED
};

class INode
{
public:
    INode() noexcept = default;

    virtual ~INode() noexcept = default;

    auto OpenList(bool value) -> void { open_list = value; }
    auto OpenList() const -> bool { return open_list; }

    auto ClosedList(bool value) -> void { closed_list = value; }
    auto ClosedList() const -> bool { return closed_list; }

    auto TotalCost() const -> int64_t { return movement_cost + estimated_cost; }
    auto MovementCost() const -> int64_t { return movement_cost; }
    virtual auto MovementCost(INode& parent) -> void = 0;
    auto EstimatedCost() const -> int64_t { return estimated_cost; }
    virtual auto EstimatedCost(INode& goal) -> void = 0;

    auto HasParent() const -> bool { return parent != nullptr; }
    auto Parent() -> INode& { return *parent; }
    auto Parent() const -> const INode& { return *parent; }
    auto Parent(INode& node) -> void { parent = &node; }

    virtual auto Children() -> std::vector<INode*>& = 0;

    virtual auto IsGoal(INode& goal) const -> bool = 0;

protected:
    int64_t movement_cost{0};
    int64_t estimated_cost{0};
    std::optional<std::vector<INode*>> children{};

private:
    bool closed_list{false};
    bool open_list{false};
    INode* parent{nullptr};
};

class AStar
{
public:
    AStar(
        INode& start_node,
        INode& goal_node
    )
        :   start(start_node),
            goal(goal_node),
            current(&start)
    {
        current->OpenList(true);
        open_list.emplace(0, *current);
    }

    auto OpenList() const -> const std::multimap<int64_t, INode&>& { return open_list; }
    auto ClosedList() const -> const std::multimap<int64_t, INode&>& { return closed_list; }

    auto CurrentNode() const -> const INode& { return *current; }

    auto Run() -> State
    {
        while(true)
        {
            auto state = Step();
            if(state != State::SEARCHING)
            {
                return state;
            }
        }
    }

    auto Step() -> State
    {
        while(true)
        {
            // If there are no more nodes to search, return failure.
            if(open_list.empty())
            {
                return State::FAILED;
            }

            current = &open_list.begin()->second;
            open_list.erase(open_list.begin());
            current->OpenList(false);

            // This node has already been searched, move to the next one.
            if(current->ClosedList())
            {
                continue;
            }

            // An unsearched node has been found, lets follow it.
            break;
        }

        current->ClosedList(true);
        closed_list.emplace(current->TotalCost(), *current);

        if(current->IsGoal(goal))
        {
            return State::GOALFOUND;
        }

        for(auto& child : current->Children())
        {
            // Skip nodes that will be searched or have been searched.
            if(child->OpenList() || child->ClosedList())
            {
                continue;
            }

            child->Parent(*current);
            child->MovementCost(*current);
            child->EstimatedCost(goal);
            open_list.emplace(child->TotalCost(), *child);
            child->OpenList(true);
        }

        return State::SEARCHING;
    }

    auto Path() -> std::vector<INode*>
    {
        std::vector<INode*> path{};

        auto* next = current;
        while(next != nullptr)
        {
            path.emplace_back(next);
            if(next->HasParent())
            {
                next = &next->Parent();
            }
            else
            {
                next = nullptr;
            }
        }

        std::reverse(path.begin(), path.end());

        return path;
    }

private:
    std::multimap<int64_t, INode&> open_list;
    std::multimap<int64_t, INode&> closed_list;

    INode& start;
    INode& goal;
    INode* current{nullptr};
};

} // namespace astar
