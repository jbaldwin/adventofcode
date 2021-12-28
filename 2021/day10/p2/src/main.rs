use aoc_core;

fn flip(c: char) -> char {
    match c {
        '(' => ')',
        '[' => ']',
        '{' => '}',
        '<' => '>',
        _ => panic!("cannot flip {}", c),
    }
}

fn points(c: char) -> u64 {
    match c {
        ')' => 3,
        ']' => 57,
        '}' => 1197,
        '>' => 25137,
        _ => panic!("cannot assign points to {}", c),
    }
}

fn parse_sequence(s: &String) -> u64 {
    let mut stack: Vec<char> = Vec::new();
    for a in s.chars() {
        match a {
            '(' | '[' | '{' | '<' => {
                stack.push(a);
            }
            _ => {
                let b = flip(stack.pop().unwrap());
                if a != b {
                    return points(a);
                }
            }
        }
    }
    return 0;
}

fn chunk_value(c: char) -> u64 {
    match c {
        '(' => 1,
        '[' => 2,
        '{' => 3,
        '<' => 4,
        _ => panic!("invalid state {}", c),
    }
}

fn complete_sequence(s: &String) -> u64 {
    let mut points: u64 = 0;
    let mut stack: Vec<char> = Vec::new();

    //These are valid but incomplete sequences, fill out the stack to the end.
    for a in s.chars() {
        match a {
            '(' | '[' | '{' | '<' => stack.push(a),
            _ => {
                stack.pop().unwrap();
            }
        }
    }

    // Pop off and count up the points
    while let Some(a) = stack.pop() {
        points *= 5;
        points += chunk_value(a);
    }

    points
}

fn main() {
    let args = aoc_core::args_init(2);
    let lines = aoc_core::file_read_delim(&args[1], '\n');

    let mut incomplete_sequences: Vec<String> = Vec::new();
    for line in lines.iter() {
        if parse_sequence(&line) == 0 {
            incomplete_sequences.push(line.clone());
        }
    }

    let mut seq_points: Vec<u64> = Vec::new();
    for seq in incomplete_sequences {
        seq_points.push(complete_sequence(&seq));
    }

    seq_points.sort();
    println!("{}", seq_points[seq_points.len() / 2]);
}
