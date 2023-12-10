use aoc_core;

use std::collections::HashMap;
use std::collections::HashSet;
use std::collections::VecDeque;

#[derive(PartialEq)]
struct Element
{
    left: String,
    right: String,
}

fn main()
{
    let mut lines = aoc_core::input_to_lines();

    let instructions = lines[0].chars().collect::<Vec<_>>();

    let mut elements = HashMap::new();

    for line in lines.iter_mut().skip(2)
    {
        line.remove(line.len() - 1);
        let parts = aoc_core::str_split_str(line.as_str(), " = (");
        let lr_parts = aoc_core::str_split_str(parts[1].as_str(), ", ");

        let e = parts[0].clone();
        let left = lr_parts[0].clone();
        let right = lr_parts[1].clone();

        elements.insert(e, Element { left, right });
    }

    let nodes = elements.iter().filter(|(k, _)| k.ends_with('A')).collect::<Vec<_>>();

    let mut lcm = instructions.len(); // The solution to part 2 is the lcm of the instructions size and each graph's cyclical length in steps.

    let mut todo = VecDeque::new();
    let mut seen = HashSet::new();

    for (k, v) in nodes.iter()
    {
        todo.push_back(((k.as_str(), *v), 0));
        seen.insert(k.as_str());

        while let Some(((k1, v1), steps)) = todo.pop_front()
        {
            // Once we find thi graphs ending 'Z' element calculate the lcm with the running lcm.
            if k1.ends_with('Z')
            {
                lcm = aoc_core::lcm(lcm, steps);
                break;
            }

            // If we haven't seen this node before search it.
            if seen.insert(v1.left.as_str())
            {
                todo.push_back(((v1.left.as_str(), &elements[v1.left.as_str()]), steps + 1));
            }

            if seen.insert(v1.right.as_str())
            {
                todo.push_back(((v1.right.as_str(), &elements[v1.right.as_str()]), steps + 1));
            }
        }

        todo.clear();
        seen.clear();
    }

    println!("{}", lcm);

}
