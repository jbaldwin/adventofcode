use aoc_core;
use pathfinding::prelude::dijkstra;

type Grid = Vec<Vec<u64>>;

#[derive(Clone, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
struct Pos(i64, i64);

impl Pos {
    fn successors(&self, grid: &Grid, neighbors: &Vec<Pos>) -> Vec<(Pos, i64)> {
        let &Pos(sx, sy) = self;

        let mut children: Vec<(Pos, i64)> = Vec::new();

        for n in neighbors.iter() {
            let x_pos = sx + n.0;
            let y_pos = sy + n.1;

            if x_pos >= 0 && x_pos < grid.len() as i64 && y_pos > 0 && y_pos < grid[0].len() as i64
            {
                children.push((
                    Pos(x_pos, y_pos),
                    grid[x_pos as usize][y_pos as usize] as i64,
                ))
            }
        }

        children
    }
}

fn main() {
    let args = aoc_core::args_init(2);
    let lines = aoc_core::file_read_delim(&args[1], '\n');

    let mut grid: Grid = Vec::new();
    for line in lines.iter() {
        grid.push(Vec::new());
        let row = grid.last_mut().unwrap();

        for c in line.chars() {
            row.push(c.to_digit(10).unwrap() as u64);
        }
    }

    let x_len_start = grid.len();
    let y_len_start = grid[0].len();

    let extend_x_amt = grid.len() * 5 - grid.len();
    let extend_y_amt = grid[0].len() * 5;

    for row in grid.iter_mut() {
        row.resize(extend_y_amt, 0);
    }

    for _ in 0..extend_x_amt {
        grid.push(Vec::new());
        let row = grid.last_mut().unwrap();
        row.resize(extend_y_amt, 0);
    }

    // Fill out to the right on the starting
    for x in 0..x_len_start {
        for y in x_len_start..grid[0].len() {
            grid[x][y] = (grid[x][y - y_len_start] + 1) % 10;
            if grid[x][y] == 0 {
                grid[x][y] = 1;
            }
        }
    }

    // Fill down to complete the grid.
    for x in x_len_start..grid.len() {
        for y in 0..grid[0].len() {
            grid[x][y] = (grid[x - x_len_start][y] + 1) % 10;
            if grid[x][y] == 0 {
                grid[x][y] = 1;
            }
        }
    }

    let neighbors: Vec<Pos> = vec![Pos(-1, 0), Pos(1, 0), Pos(0, -1), Pos(0, 1)];

    let goal: Pos = Pos((grid.len() - 1) as i64, (grid[0].len() - 1) as i64);
    let result = dijkstra(
        &Pos(0, 0),
        |p| p.successors(&grid, &neighbors),
        |p| *p == goal,
    );
    match result {
        Some((_path, cost)) => {
            println!("{}", (cost - grid[0][0] as i64));
        }
        None => {
            println!("not found");
            // rip
        }
    }
}
