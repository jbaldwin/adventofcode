use aoc_core;

use std::collections::HashMap;

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

    let mut current = "AAA";
    let mut i = 0;
    let mut steps = 0;
    loop
    {
        let e = &elements[current];
        current = match instructions[i]
        {
            'L' => e.left.as_str(),
            'R' => e.right.as_str(), 
            _ => panic!("invalid instruction"),
        };

        steps += 1;
        i = (i + 1) % instructions.len();

        if current == "ZZZ"
        {
            break;
        }
    }

    println!("{}", steps);

}
