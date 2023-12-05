use aoc_core;

fn parse_number(row: &Vec<char>, x: usize) -> (i64, usize)
{
    let mut s = String::new();
    let mut i = x;
    while i < row.len() && row[i].is_digit(10)
    {
        s.push(row[i]);
        i += 1;
    }

    let n = s.parse::<i64>().unwrap();
    (n, s.len())
}

fn is_machine_part(grid: &Vec<Vec<char>>, x: usize, y: usize, mut len: usize) -> bool
{
    let mut x_start = x;
    if x_start > 0
    {
        x_start -= 1;
        len += 2;
    }
    else
    {
        len += 1;
    }

    let x_stop = x_start + len;

    let y_start: i64 = if y > 0
    {
        -1
    }
    else
    {
        0
    };

    for i in x_start..x_stop
    {
        for y_diff in y_start..=1
        {
            let j = ((y as i64) + y_diff) as usize;

            if i < grid.len() && j < grid[0].len()
            {
                match grid[j][i] {
                    '0'..='9' => continue,
                    '.' => continue,
                    _ => return true,
                };
            }
        }
    }

    false
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
        let mut skip = false;
        for (x, c) in row.iter().enumerate()
        {
            match c
            {
                '0'..='9' => {
                    if skip
                    {
                        continue;
                    }
                    let (n, len) = parse_number(row, x);
                    if is_machine_part(&grid, x, y, len)
                    {
                        sum += n;
                    }
                    skip = true;
                },
                _ => { 
                    skip = false;
                },
            }
        }
    }

    println!("{}", sum);
}
