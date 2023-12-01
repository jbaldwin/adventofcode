use aoc_core;
use std::time::Instant;


fn main() {
    let now = Instant::now();
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');
}
