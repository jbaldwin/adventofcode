use aoc_core;
use std::collections::HashMap;

type PairInsertionRules = HashMap<(char, char), char>;

fn main() {
    let args = aoc_core::args_init(3);
    let steps = args[2].parse::<usize>().unwrap();
    let lines = aoc_core::file_read_delim(&args[1], '\n');
    let mut lines_iter = lines.iter();

    let mut template = lines_iter.next().unwrap().clone();
    lines_iter.next(); // skip blank line

    let mut rules: PairInsertionRules = HashMap::new();

    for line in lines_iter {
        let parts = aoc_core::str_split_str(line, " -> ");
        let mut iter = parts[0].chars();
        rules.insert(
            (iter.next().unwrap(), iter.next().unwrap()),
            parts[1].chars().next().unwrap(),
        );
    }

    // println!("Template:     {}", &template);

    for step in 1..=steps {
        println!("Step {}", step);
        let mut next = String::new();
        next.reserve(template.len() * 2);

        let mut char_iter = template.chars().peekable();

        loop {
            if let Some(key_a) = char_iter.next() {
                if let Some(key_b) = char_iter.peek() {
                    let key = (key_a, *key_b);
                    if let Some(c) = rules.get(&key) {
                        next.push(key_a);
                        next.push(*c);
                    } else {
                        next.push(key_a);
                    }
                } else {
                    next.push(key_a);
                }
            } else {
                break;
            }
        }

        template = next;
        // println!("After step {}: {}", step, &template);
    }

    let mut counts: HashMap<char, u64> = HashMap::new();
    for c in template.chars() {
        *counts.entry(c).or_insert(0) += 1;
    }

    let mut min: u64 = u64::MAX;
    let mut max: u64 = 0;

    for (k, v) in counts {
        println!("{} = {}", k, v);

        min = std::cmp::min(min, v);
        max = std::cmp::max(max, v);
    }

    println!("{}", (max - min));
}
