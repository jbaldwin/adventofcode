use aoc_core;
use std::collections::HashSet;

fn to_priority(c : char) -> u32 {
    match c {
        'A'..='Z' => {
            c as u32 - 65 + 27
        },
        'a'..='z' => {
            c as u32 - 96
        },
        _ => panic!("invalid input")
    }
}

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');

    let mut rucksacks = Vec::new();
    for line in lines.iter() {
        let mut compartment1 = Vec::new();
        let mut compartment2 = HashSet::new();

        for (i, c) in line.chars().enumerate() {
            if i < line.len() / 2 {
                compartment1.push(c);
            }
            else {
                compartment2.insert(c);
            }
        }

        rucksacks.push((compartment1, compartment2));
    }

    let mut common = Vec::new();
    for rucksack in rucksacks.iter() {
        let (c1, c2) = rucksack;

        for c in c1.iter() {
            if c2.contains(c) {
                common.push(c);
                break; // only 1 item per rucksack should match
            }
        }
    }

    let sum: u32 = common.iter().map(|n| to_priority(**n)).sum();
    println!("{}", sum);


}
