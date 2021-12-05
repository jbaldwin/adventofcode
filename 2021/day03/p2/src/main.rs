fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 2 {
        println!("{} <input_file>", args[0]);
        std::process::exit(-1);
    }

    let og_numbers: Vec<String> = std::fs::read_to_string(&args[1])
        .unwrap()
        .split('\n')
        .into_iter()
        .map(|s| s.to_string())
        .collect();

    let mut numbers = og_numbers.clone();

    let mut col: usize = 0;
    while numbers.len() > 1 {
        let mut winner: char = '0';
        let mut zeros: i64 = 0;
        let mut ones: i64 = 0;

        for number in numbers.iter() {
            let c = number.as_bytes()[col] as char;

            match c {
                '0' => zeros += 1,
                '1' => ones += 1,
                _ => panic!("Unknown char {} in number {}", c, &number),
            }

            winner = if zeros > ones { '0' } else { '1' };
        }

        println!(
            "winner={}, zeros={}, ones={}, col={}",
            winner, zeros, ones, col
        );

        let mut keep: Vec<String> = Vec::new();
        keep.reserve(numbers.len());
        for number in numbers.into_iter() {
            let c = number.as_bytes()[col] as char;
            if c == winner {
                println!("keep={}", number);
                keep.push(number);
            } else {
                println!("discard={}", number);
            }
        }
        println!();

        numbers = keep;
        col += 1;
    }

    let radix = 2;
    let oxygen_generator_rating: i64 = i64::from_str_radix(&numbers[0], radix).unwrap();

    println!("{} {}", numbers[0], oxygen_generator_rating);

    ///////
    let mut numbers = og_numbers.clone();

    let mut col: usize = 0;
    while numbers.len() > 1 {
        let mut winner: char = '0';
        let mut zeros: i64 = 0;
        let mut ones: i64 = 0;

        for number in numbers.iter() {
            let c = number.as_bytes()[col] as char;

            match c {
                '0' => zeros += 1,
                '1' => ones += 1,
                _ => panic!("Unknown char {} in number {}", c, &number),
            }

            winner = if zeros > ones { '1' } else { '0' };
        }

        println!(
            "winner={}, zeros={}, ones={}, col={}",
            winner, zeros, ones, col
        );

        let mut keep: Vec<String> = Vec::new();
        keep.reserve(numbers.len());
        for number in numbers.into_iter() {
            let c = number.as_bytes()[col] as char;
            if c == winner {
                println!("keep={}", number);
                keep.push(number);
            } else {
                println!("discard={}", number);
            }
        }
        println!();

        numbers = keep;
        col += 1;
    }

    let radix = 2;
    let co2_scrubber_rating: i64 = i64::from_str_radix(&numbers[0], radix).unwrap();

    println!("{} {}", numbers[0], co2_scrubber_rating);

    println!("product={}", oxygen_generator_rating * co2_scrubber_rating);
}
