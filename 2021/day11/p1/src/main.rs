use aoc_core;

type Grid = Vec<Vec<u64>>;

fn print_grid(grid: &Grid) {
    for row in grid.iter() {
        for col in row.iter() {
            print!("{}", col);
        }
        println!();
    }
}

fn step(grid: &mut Grid) -> u64 {
    for x in 0..grid.len() {
        for y in 0..grid[0].len() {
            grid[x][y] += 1;
        }
    }

    let mut flashed = true;
    while flashed {
        flashed = false;
        for x in 0..grid.len() {
            for y in 0..grid[0].len() {
                if grid[x][y] > 9 {
                    grid[x][y] = 0;
                    
                    for dx in -1..=1 {
                        for dy in -1..=1 {
                            if dx == 0 && dy == 0 {
                                continue;
                            }

                            let nx = (x as i64) + dx;
                            let ny = (y as i64) + dy;

                            if nx >= 0 && nx < grid.len() as i64 &&
                               ny >= 0 && ny < grid[0].len() as i64 {
                                let octo: &mut u64 = &mut grid[nx as usize][ny as usize];

                                if *octo > 0 {
                                    *octo += 1;
                                }
                            }
                        }
                    }

                    flashed = true;
                }
            }
        }
    }

    let mut flashes: u64 = 0;
    for x in 0..grid.len() {
        for y in 0..grid[0].len() {
            if grid[x][y] == 0 {
                flashes += 1;
            }
        }
    }
    flashes
}


fn main() {
    let args = aoc_core::args_init(3);
    let lines = aoc_core::file_read_delim(&args[1], '\n');
    let total_steps = args[2].parse::<i64>().unwrap();

    let mut octopuses: Grid = Vec::new();

    for line in lines.iter() {
        octopuses.push(Vec::new());
        let row = octopuses.last_mut().unwrap();
        for c in line.chars() {
            row.push(c.to_digit(10).unwrap() as u64);
        }
    }

    println!("Before any steps:");
    print_grid(&octopuses);
    println!();

    let mut flashes: u64 = 0;
    for s in 1..=total_steps {
        flashes += step(&mut octopuses);
        println!("After step {}:", s);
        print_grid(&octopuses);
        println!();
    }

    println!("{}", flashes);
}
