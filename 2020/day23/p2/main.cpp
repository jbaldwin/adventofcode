#include <lib/file_util.hpp>
#include <chain/chain.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <tuple>

const constexpr int64_t one_million = 1'000'000;

struct node
{
    int64_t value{0};
    node* next{nullptr};
};

auto two_cups_one_label(const node* start) -> std::pair<int64_t, int64_t>
{
    const node* n = start;
    while(n->value != 1)
    {
        n = n->next;
    }
    n = n->next; // skip 1, and then print

    return {n->value, n->next->value};
}

auto picked_cups(const node* start) -> std::tuple<node*, node*, node*>
{
    node* p1 = start->next;
    node* p2 = p1->next;
    node* p3 = p2->next;
    return std::make_tuple(p1, p2, p3);
}

auto destination_cup_value(
    const node* start,
    const node* p1,
    const node* p2,
    const node* p3,
    const int64_t below_min_value,
    const int64_t max_value
) -> int64_t
{
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
    return dst_cup_value;
}

/**
 * @param contents The input file contents.
 * @return The ring buffer of nodes linked together with a forward list of pointers.
 *         A node value -> node* map to quickly lookup nodes via their values.
 *         Min value.
 *         Max value.
 */
auto parse_input(std::string_view contents) -> std::tuple<std::vector<node>, std::vector<node*>, int64_t, int64_t>
{
    int64_t min_value{std::numeric_limits<int64_t>::max()};
    int64_t max_value{0};

    // For part two create a map of value -> node* to quickly lookup the destination cup.
    // Without this its a O(n) scan across the cups to find it, which is absymally slow!
    std::vector<node*> value_to_node{};
    value_to_node.resize(one_million + 1, nullptr);

    std::vector<node> cups{};
    cups.reserve(one_million);
    for(const char c : contents)
    {
        int64_t value = static_cast<int64_t>(c - '0');
        min_value = std::min(value, min_value);
        max_value = std::max(value, max_value);

        cups.emplace_back(value, nullptr);
        value_to_node[value] = &cups.back();
    }

    for(int64_t i = max_value + 1; i <= one_million; ++i)
    {
        cups.emplace_back(i, nullptr);
        value_to_node[i] = &cups.back();
    }

    // Max value will always be 1 million for this problem, but we need to know what the input files
    // max value is to properly count up to 1 million.
    max_value = one_million;

    // Make the ring buffer.
    for(size_t i = 0; i < cups.size() - 1; ++i)
    {
        cups[i].next = &cups[i + 1];
    }
    cups[cups.size() - 1].next = &cups[0];

    return std::make_tuple(
        std::move(cups),
        std::move(value_to_node),
        min_value,
        max_value
    );
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

    auto [cups, value_to_node, min_value, max_value] = parse_input(contents);
    auto below_min_value = min_value - 1;

    node* start = &cups[0];

    for(size_t move = 1; move <= 10 * one_million; ++move)
    {
        auto [p1, p2, p3] = picked_cups(start);
        auto dst_cup_value = destination_cup_value(start, p1, p2, p3, below_min_value, max_value);

        // Splice out the 3 nodes.
        start->next = p3->next;

        // Get the destination node.
        node* dst_node = value_to_node[dst_cup_value];

        // Splice in the 3 nodes (they are already chained amonst themselves).
        node* after = dst_node->next;
        dst_node->next = p1;
        p3->next = after;

        // Go to the next move starting position.
        start = start->next;
    }

    auto [c1, c2] = two_cups_one_label(start);
    std::cout << c1 << " * " << c2 << " = " << (c1 * c2) << "\n";

    return 0;
}
