fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 2 {
        println!("{} <input_file>", args[0]);
        std::process::exit(-1);
    }

    let numbers: Vec<String> = std::fs::read_to_string(&args[1])
        .unwrap()
        .split('\n')
        .into_iter()
        .map(|s| s.to_string())
        .collect();

    // Convert to columnar format.
    let len = numbers[0].len();
    let mut columns: Vec<String> = Vec::new();
    columns.resize(len, String::new());

    for number in numbers.iter() {
        let mut i: usize = 0;
        for c in number.chars() {
            columns[i].push(c);
            i += 1;
        }
    }

    // Iterate each column's values and sum up zeros and ones.
    let mut gamma: String = String::new();
    let mut epsilon: String = String::new();

    for column in columns.iter() {
        let mut zeros: i64 = 0;
        let mut ones: i64 = 0;
        for c in column.chars() {
            match c {
                '0' => zeros += 1,
                '1' => ones += 1,
                _ => panic!("Unknown char {} in number {}", c, &column),
            }
        }

        // Append appropriately based on which value is greater.
        let (c1, c2) = if zeros > ones { ('0', '1') } else { ('1', '0') };

        gamma.push(c1);
        epsilon.push(c2);
    }

    let radix = 2;
    let gamma_i: i64 = i64::from_str_radix(&gamma, radix).unwrap();
    let epsilon_i: i64 = i64::from_str_radix(&epsilon, radix).unwrap();

    println!(
        "gamma={}, epsilon={}, product={}",
        gamma,
        epsilon,
        gamma_i * epsilon_i
    );
}
