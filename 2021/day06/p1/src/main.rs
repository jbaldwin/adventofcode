fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 3 {
        println!("{} <input_file> <days>", args[0]);
        std::process::exit(-1);
    }

    let days: i64 = args[2].parse::<i64>().unwrap();

    let starting_lanternfish: Vec<i64> = std::fs::read_to_string(&args[1])
        .unwrap()
        .split(',')
        .into_iter()
        .map(|s| s.parse::<i64>().unwrap())
        .collect();

    let mut lf1: Vec<i64> = vec![0, 0, 0, 0, 0, 0, 0, 0, 0];
    let mut lf2: Vec<i64> = vec![0, 0, 0, 0, 0, 0, 0, 0, 0];

    for fish in starting_lanternfish {
        lf1[fish as usize] += 1;
    }

    for _ in 0..days {
        // Rotate
        lf2[0] = lf1[1];
        lf2[1] = lf1[2];
        lf2[2] = lf1[3];
        lf2[3] = lf1[4];
        lf2[4] = lf1[5];
        lf2[5] = lf1[6];
        lf2[6] = lf1[7];
        lf2[7] = lf1[8];

        // Breeding rules.
        lf2[6] += lf1[0];
        lf2[8] = lf1[0];

        // Swap and reset.
        lf1 = lf2;
        lf2 = vec![0, 0, 0, 0, 0, 0, 0, 0, 0];
    }

    let mut count: i64 = 0;
    for i in 0..9 {
        println!("{}->{}", i, lf1[i]);
        count += lf1[i];
    }

    println!("After {} days there are {} lanternfish.", days, count);
}
