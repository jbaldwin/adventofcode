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

fn main() {
    let args = aoc_core::args_init(2);
    let lines = aoc_core::file_read_delim(&args[1], '\n');

    let mut points: u64 = 0;

    for line in lines.iter() {
        points += parse_sequence(&line);
    }

    println!("{}", points);
}
