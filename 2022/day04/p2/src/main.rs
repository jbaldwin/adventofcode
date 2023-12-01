use aoc_core;

struct Pair {
    start: u64,
    end: u64,
}

impl Pair {
    fn intersects(&self, b: &Pair) -> bool {
        !(b.end < self.start || self.end < b.start)
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

    let mut intersects = Vec::new();

    for (p1, p2) in assignments.iter() {
        if p1.intersects(&p2) {
            intersects.push((p1, p2));
        }
    }

    println!("{}", intersects.len());

}
