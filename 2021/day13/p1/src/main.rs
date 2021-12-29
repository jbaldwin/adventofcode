use aoc_core;

type Paper = Vec<Vec<char>>;

#[derive(PartialEq)]
enum Problem {
    P1 = 1,
    P2 = 2,
}

#[derive(Debug)]
enum Axis {
    X,
    Y,
}

#[derive(Debug)]
struct Fold {
    axis: Axis,
    unit: usize,
}

fn print_paper(paper: &Paper) {
    for row in paper.iter() {
        for col in row.iter() {
            print!("{}", col);
        }
        println!();
    }
    println!();
}

fn paper_dots_sum(paper: &Paper) -> u64 {
    let mut sum: u64 = 0;
    for row in paper.iter() {
        for col in row.iter() {
            if *col == '#' {
                sum += 1;
            }
        }
    }
    sum
}

fn main() {
    let args = aoc_core::args_init(3);
    let lines = aoc_core::file_read_delim(&args[1], '\n');
    let problem: Problem = if args[2] == "1" {
        Problem::P1
    } else {
        Problem::P2
    };

    let mut points_part = true;
    let mut paper: Paper = Vec::new();

    // Need to fill out all rows to have the same column width.
    let mut max_x: usize = 0;
    // let mut max_y: usize = 0;

    let mut folds: Vec<Fold> = Vec::new();

    for line in lines.iter() {
        if line.len() == 0 {
            points_part = false;
            continue;
        }

        if points_part {
            let parts = aoc_core::str_split_char(line, ',');

            let x: usize = parts[0].parse::<usize>().unwrap();
            let y: usize = parts[1].parse::<usize>().unwrap();

            while y >= paper.len() {
                paper.push(Vec::new());
            }

            while x >= paper[y].len() {
                paper[y].push(' ');
            }

            paper[y][x] = '#';

            // max_y = std::cmp::max(max_y, y + 1);
            max_x = std::cmp::max(max_x, x + 1);
        } else {
            let spaces_parts = aoc_core::str_split_char(line, ' ');
            let equals_parts = aoc_core::str_split_char(&spaces_parts[2], '=');

            let axis = if equals_parts[0].chars().next().unwrap() == 'x' {
                Axis::X
            } else {
                Axis::Y
            };
            let unit = equals_parts[1].parse::<usize>().unwrap();

            folds.push(Fold { axis, unit });
        }
    }

    // Populate all columns.
    for row in paper.iter_mut() {
        row.resize(max_x, ' ');
    }

    // print_paper(&paper);

    for fold in folds.iter() {
        match fold.axis {
            Axis::X => {
                // {
                //     for y in 0..paper.len() {
                //         let row = &mut paper[y];
                //         row[fold.unit] = '|';
                //     }
                // }

                let mut from_x = fold.unit + 1;
                while from_x < paper[0].len() {
                    let y_len = paper.len();

                    let diff = from_x - fold.unit;
                    let to_x = fold.unit - diff;

                    for i in 0..y_len {
                        if paper[i][from_x] == '#' {
                            paper[i][to_x] = paper[i][from_x];
                        }
                    }

                    from_x += 1;
                }

                for row in paper.iter_mut() {
                    row.truncate(fold.unit);
                }
            }
            Axis::Y => {
                // {
                //     let row = &mut paper[fold.unit];
                //     for col in row.iter_mut() {
                //         *col = '-'
                //     }
                // }

                let mut from_y = fold.unit + 1;
                while from_y < paper.len() {
                    let x_len = paper[from_y].len();

                    let diff = from_y - fold.unit;
                    let to_y = fold.unit - diff;

                    for i in 0..x_len {
                        if paper[from_y][i] == '#' {
                            paper[to_y][i] = paper[from_y][i];
                        }
                    }

                    from_y += 1;
                }

                paper.truncate(fold.unit);
            }
        }

        // print_paper(&paper);

        // p1 only requires the first fold.
        if problem == Problem::P1 {
            break;
        }
    }

    print_paper(&paper);
    let visible_dots = paper_dots_sum(&paper);
    println!("Visible dots {}", visible_dots);
}
