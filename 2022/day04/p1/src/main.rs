use aoc_core;

struct Pair {
    start: u64,
    end: u64,
}

impl Pair {
    fn fully_contains(&self, b: &Pair) -> bool {
        self.start <= b.start && self.end >= b.end
    }
}

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');

    let mut assignments = Vec::new();

    for line in lines.iter() {
        let dash_parts = aoc_core::str_split_char(&line, ',');
        let p1_parts = aoc_core::str_split_char(&dash_parts[0], '-');
        let p2_parts = aoc_core::str_split_char(&dash_parts[1], '-');

        assignments.push((Pair {
            start: p1_parts[0].parse::<u64>().unwrap(),
            end: p1_parts[1].parse::<u64>().unwrap(),
        },
        Pair {
            start: p2_parts[0].parse::<u64>().unwrap(),
            end: p2_parts[1].parse::<u64>().unwrap(),
        }));
    }

    let mut contained = Vec::new();

    for (p1, p2) in assignments.iter() {
        if p1.fully_contains(&p2) {
            contained.push(p2);
        } else if p2.fully_contains(&p1) {
            contained.push(p1);
        }
    }

    println!("{}", contained.len());
}
