fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 2 {
        println!("{} <input_file>", args[0]);
        std::process::exit(-1);
    }

    let depths: Vec<i64> = std::fs::read_to_string(&args[1])
        .unwrap()
        .split('\n')
        .into_iter()
        .map(|s| s.parse::<i64>().unwrap())
        .collect();

    let mut previous_depth: i64 = 0;
    let mut count: i64 = -1; // So the first item is negated.
    for depth in depths {
        if depth > previous_depth {
            count += 1;
        }
        previous_depth = depth;
    }

    println!("{}", count);
}
