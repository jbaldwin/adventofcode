use aoc_core;
use std::collections::HashSet;
use std::collections::HashMap;

type Caves = HashMap<String, HashSet<String>>;

fn pathfind(
    caves: &Caves,
    current: String,
    mut visited: Vec<String>,
    paths: &mut Vec<Vec<String>>) {

    if current == "end" {
        visited.push(current);
        paths.push(visited);
        return;
    }

    visited.push(current.clone());

    let connected = caves.get(&current).unwrap();

    for next in connected.iter() {
        // Little caves cannot be visited twice, if the little cave is in
        // the current path, e.g. already visisted, then continue to the next.
        let first_letter = next.chars().next().unwrap();
        if first_letter.to_ascii_lowercase() == first_letter {
            if visited.contains(next) {
                continue;
            }
        }

        pathfind(caves, next.clone(), visited.clone(), paths);
    }
}

fn main() {
    let args = aoc_core::args_init(2);
    let lines = aoc_core::file_read_delim(&args[1], '\n');

    let mut caves: Caves = HashMap::new();
    for line in lines.iter() {
        let parts: Vec<String> = line.split('-')
            .into_iter()
            .map(|s| s.to_string())
            .collect();
        
        caves.entry(parts[0].clone()).or_insert(HashSet::new()).insert(parts[1].clone());
        caves.entry(parts[1].clone()).or_insert(HashSet::new()).insert(parts[0].clone());
    }

    for (cave, connected) in caves.iter() {
        print!("{} - ", cave);
        for c in connected.iter() {
            print!("{}, ", c);
        }
        println!();
    }
    println!("Starting to find paths...\n\n");

    let mut paths: Vec<Vec<String>> = Vec::new();
    pathfind(&caves, "start".to_string(), Vec::new(), &mut paths);

    for path in paths.iter() {
        for c in path.iter() {
            print!("{},", c);
        }
        println!();
    }
    println!();

    println!("Number of paths {}", paths.len());
}
