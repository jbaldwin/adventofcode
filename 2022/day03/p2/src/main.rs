use aoc_core;
use std::collections::HashSet;
use itertools::Itertools;

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
        let mut rucksack = HashSet::new();

        for c in line.chars() {
            rucksack.insert(c);
        }

        rucksacks.push(rucksack);
    }

    let groups = rucksacks.chunks(3);

    let mut badges = Vec::new();
    for group in groups {
        let (r1, r2, r3) = group.iter().collect_tuple().unwrap();

        for c in r1.iter() {
            if r2.contains(c) && r3.contains(c) {
                badges.push(c);
                break;
            }
        }
    }

    let sum: u32 = badges.iter().map(|n| to_priority(**n)).sum();
    println!("{}", sum);


}
