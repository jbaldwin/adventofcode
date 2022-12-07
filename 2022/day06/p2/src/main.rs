use aoc_core;

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();

    for i in 13..contents.len() {
        let slice = &contents[i-13..=i];

        let mut found = true;
        'outer: for x in 0..13 {
            for y in (x+1)..14 {
                let a = slice.chars().nth(x).unwrap();
                let b = slice.chars().nth(y).unwrap();

                if a == b {
                    found = false;
                    break 'outer;
                }
            }
        }

        if found {
            println!("{} -> {}", (i + 1), slice);
            return;
        }
    }
}
