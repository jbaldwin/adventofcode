use ansi_term::Style;

struct Board {
    rows: Vec<Vec<(i64, bool)>>,
}

impl Board {
    fn new() -> Board {
        Board { rows: Vec::new() }
    }

    fn add_row(&mut self, row: &str) {
        self.rows.push(Vec::new());
        let last = self.rows.last_mut().unwrap();
        for s in row.split(' ') {
            if s.len() > 0 {
                let n = s.parse::<i64>().unwrap();
                last.push((n, false));
            }
        }
    }

    fn number_drawn(&mut self, n: i64) {
        for row in self.rows.iter_mut() {
            for num in row.iter_mut() {
                if num.0 == n {
                    num.1 = true;
                }
            }
        }
    }

    fn has_won(&self) -> bool {
        for row in self.rows.iter() {
            let mut count: usize = 0;
            for num in row.iter() {
                if num.1 == true {
                    count = count + 1;
                }
            }
            if count == row.len() {
                return true;
            }
        }

        for col in 0..self.rows.len() {
            let mut count: usize = 0;
            for row in 0..self.rows.len() {
                if self.rows[row][col].1 == true {
                    count = count + 1;
                }
            }
            if count == self.rows.len() {
                return true;
            }
        }

        false
    }

    fn sum_undrawn(&self) -> i64 {
        let mut total: i64 = 0;
        for row in self.rows.iter() {
            for num in row.iter() {
                if num.1 == false {
                    total = total + num.0;
                }
            }
        }
        total
    }
}

impl std::fmt::Display for Board {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        for row in self.rows.iter() {
            for n in row {
                if n.1 {
                    write!(f, "{} ", Style::new().bold().paint(n.0.to_string()))?
                } else {
                    write!(f, "{} ", n.0)?
                }
            }
            writeln!(f)?
        }
        std::fmt::Result::Ok(())
    }
}

fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 2 {
        println!("{} <input_file>", args[0]);
        std::process::exit(-1);
    }

    let file_parts: Vec<String> = std::fs::read_to_string(&args[1])
        .unwrap()
        .split('\n')
        .into_iter()
        .map(|s| s.to_string())
        .collect();

    let numbers: Vec<i64> = file_parts[0]
        .split(',')
        .into_iter()
        .map(|s| s.parse::<i64>().unwrap())
        .collect();

    let mut boards: Vec<Board> = Vec::new();

    for line in file_parts.iter().skip(1) {
        if line.len() == 0 {
            boards.push(Board::new());
        } else {
            boards.last_mut().unwrap().add_row(line);
        }
    }

    for n in numbers {
        for b in boards.iter_mut() {
            b.number_drawn(n);
            if b.has_won() {
                println!("{}", b);

                let sum = b.sum_undrawn();
                println!("score = {}", (n * sum));
                return;
            }
        }
    }
}
