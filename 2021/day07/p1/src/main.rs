fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 2 {
        println!("{} <input_file>", args[0]);
        std::process::exit(-1);
    }

    let crabs: Vec<i64> = std::fs::read_to_string(&args[1])
        .unwrap()
        .split(',')
        .into_iter()
        .map(|s| s.parse::<i64>().unwrap())
        .collect();

    let min: i64 = *crabs.iter().min().unwrap();
    let max: i64 = *crabs.iter().max().unwrap();

    let mut spent_fuel = std::i64::MAX;
    let mut min_position: i64 = -1;

    for i in min..=max {
        let mut total_fuel: i64 = 0;
        for c in crabs.iter() {
            let required_fuel = (i - c).abs();
            total_fuel += required_fuel;
        }

        if total_fuel < spent_fuel {
            spent_fuel = total_fuel;
            min_position = i;
        }
    }

    println!(
        "Crab position {} spending {} fuel.",
        min_position, spent_fuel
    );
}
