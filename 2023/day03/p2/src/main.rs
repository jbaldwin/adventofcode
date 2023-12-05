use aoc_core;

use std::collections::HashSet;

fn parse_number(grid: &Vec<Vec<char>>, x: usize, y: usize) -> i64
{
    let mut s = String::new();
    let mut x_pos = x;
    while x_pos > 0 && grid[y][x_pos - 1].is_digit(10)
    {
        x_pos -= 1;
    }

    while x_pos < grid[y].len() && grid[y][x_pos].is_digit(10)
    {
        s.push(grid[y][x_pos]);
        x_pos += 1;
    }

    s.parse::<i64>().unwrap()
}

fn main() {
    let lines = aoc_core::input_to_lines();

    let mut grid = Vec::new();

    for line in lines.iter()
    {
        grid.push(Vec::new());
        let row = grid.last_mut().unwrap();

        for c in line.chars()
        {
            row.push(c);
        }
    }

    let mut sum = 0;
    for (y, row) in grid.iter().enumerate()
    {
        for (x, c) in row.iter().enumerate()
        {
            match c
            {
                '*' => {
                    let mut numbers = HashSet::new();
                    for x_diff in -1..=1 as i64
                    {
                        for y_diff in -1..=1 as i64
                        {
                            let y1 = (y as i64) + y_diff;
                            let x1 = (x as i64) + x_diff;

                            if y1 >= 0 && y1 < (grid.len() as i64) && x1 >= 0 && x1 <= (grid[0].len() as i64)
                            {
                                let y1 = y1 as usize;
                                let x1 = x1 as usize;

                                let x_col = grid[y1][x1];
                                if x_col.is_digit(10)
                                {
                                    let n = parse_number(&grid, x1, y1);
                                    numbers.insert(n);
                                }
                            }
                        }
                    }

                    if numbers.len() == 2
                    {
                        sum += numbers.iter().fold(1, |a, n| a * n);
                    }
                },
                _ => { },
            }
        }
    }

    println!("{}", sum);
}
