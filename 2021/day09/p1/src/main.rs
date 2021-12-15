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

    let mut grid: Vec<Vec<i64>> = Vec::new();

    for line in lines.iter() {
        grid.push(Vec::new());
        let row = grid.last_mut().unwrap();

        for c in line.chars() {
            row.push(c.to_string().parse::<i64>().unwrap());
        }
    }

    let mut sum: i64 = 0;

    let x_len: usize = grid.len();
    let y_len: usize = grid[0].len();

    for x in 0..x_len {
        for y in 0..y_len {
            let me: i64 = grid[x][y];
            let mut higher: i64 = 0;

            if x > 0 {
                if grid[x - 1][y] > me {
                    higher += 1;
                }
            } else {
                higher += 1;
            }

            if x + 1 < x_len {
                if grid[x + 1][y] > me {
                    higher += 1;
                }
            } else {
                higher += 1;
            }

            if y > 0 {
                if grid[x][y - 1] > me {
                    higher += 1;
                }
            } else {
                higher += 1;
            }

            if y + 1 < y_len {
                if grid[x][y + 1] > me {
                    higher += 1;
                }
            } else {
                higher += 1;
            }

            if higher == 4 {
                sum += me + 1;
                println!("{}", me);
            }
        }
    }

    println!("{}", sum);
}
