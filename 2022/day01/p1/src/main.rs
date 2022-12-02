use aoc_core;

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');

    let mut max: u64 = 0;
    let mut cur: u64 = 0;

    for line in lines.iter() {
        match line.parse::<u64>() {
            Ok(i) => cur += i,
            Err(_) => {
                if cur > max {
                    max = cur;
                }
                cur = 0;
            }
        }
    }
    // Since the final item doesn't have an empty line, include it.
    if cur > max {
        max = cur;
    }

    println!("{}", max);
}
