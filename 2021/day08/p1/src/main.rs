fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 2 {
        println!("{} <input_file>", args[0]);
        std::process::exit(-1);
    }

    let lines: Vec<String> = std::fs::read_to_string(&args[1])
        .unwrap()
        .split('\n')
        .into_iter()
        .map(|s| s.to_string())
        .collect();

    let mut count: i64 = 0;
    for line in lines.iter() {
        let mut split = line.split(" | ");
        let four_str = split.nth(1).unwrap();
        let output_values = four_str.split(' ');

        for o in output_values {
            print!("{} ", o);
            if o.len() == 2 || o.len() == 3 || o.len() == 4 || o.len() == 7 {
                count += 1;
            }
        }
        println!();
    }

    println!("{}", count);
}
