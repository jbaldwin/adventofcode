use aoc_core;

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');

    let mut values = Vec::new();
    let mut cur: u64 = 0;

    for line in lines.iter() {
        match line.parse::<u64>() {
            Ok(i) => cur += i,
            Err(_) => {
                values.push(cur);
                cur = 0;
            }
        }
    }
    values.push(cur);

    values.sort();
    values.reverse();
    let total = values[0] + values[1] + values[2];
    println!("{}", total);
}
