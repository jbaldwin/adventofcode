#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>

struct node
{
    int64_t value{0};
    node* next{nullptr};
};

auto print_cups(const node* start) -> void
{
    std::cout << "cups: (" << start->value << ") ";
    node* next = start->next;
    while(next != start)
    {
        std::cout << next->value << " ";
        next = next->next;
    }
    std::cout << "\n";
}

auto print_picked_up_cups(const node* p1, const node* p2, const node* p3) -> void
{
    std::cout << "pick up: " << p1->value << ", " << p2->value << ", " << p3->value << "\n";
}

auto print_cup_labels_from_1(const node* start) -> void
{
    const node* n = start;
    while(n->value != 1)
    {
        n = n->next;
    }
    n = n->next; // skip 1, and then print
    while(n->value != 1)
    {
        std::cout << n->value;
        n = n->next;
    }
    std::cout << "\n";
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

    int64_t below_min_value{std::numeric_limits<int64_t>::max()};
    int64_t max_value{0};

    std::vector<node> cups{};
    cups.reserve(contents.size());
    for(const char c : contents)
    {
        int64_t value = static_cast<int64_t>(c - '0');
        below_min_value = std::min(value, below_min_value);
        max_value = std::max(value, max_value);

        cups.emplace_back(value, nullptr);
    }
    --below_min_value;

    // Make the ring buffer.
    for(size_t i = 0; i < cups.size() - 1; ++i)
    {
        cups[i].next = &cups[i + 1];
    }
    cups[cups.size() - 1].next = &cups[0];

    node* start = &cups[0];

    constexpr size_t max_moves = 100;

    for(size_t move = 1; move <= max_moves; ++move)
    {
        std::cout << "-- move " << move << " --\n";
        print_cups(start);

        node* p1 = start->next;
        node* p2 = p1->next;
        node* p3 = p2->next;

        print_picked_up_cups(p1, p2, p3);

        uint64_t dst_cup_value = start->value - 1;
        if(dst_cup_value == below_min_value)
        {
            dst_cup_value = max_value;
        }
        while(dst_cup_value == p1->value || dst_cup_value == p2->value || dst_cup_value == p3->value)
        {
            dst_cup_value--;
            if(dst_cup_value == below_min_value)
            {
                dst_cup_value = max_value;
            }
        }
        std::cout << "destination: " << dst_cup_value << "\n\n";

        // splice out the 3 nodes
        start->next = p3->next;

        // find the destination node
        node* dst_node = start->next;
        while(dst_node != start)
        {
            if(dst_node->value == dst_cup_value)
            {
                break;
            }
            dst_node = dst_node->next;
        }

        // splice in the 3 nodes (they are already chained amonst themselves)
        node* after = dst_node->next;
        dst_node->next = p1;
        p3->next = after;

        start = start->next;
    }

    print_cup_labels_from_1(start);

    return 0;
}
