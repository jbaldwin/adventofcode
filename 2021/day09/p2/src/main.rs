use std::collections::HashSet;

type Grid = Vec<Vec<i64>>;
type Basin = HashSet<Point>;

#[derive(Debug, Copy, Clone, Hash, Eq, PartialEq)]
struct Point {
    x: usize,
    y: usize,
}

impl Point {
    fn new(x: usize, y: usize) -> Point {
        Point { x, y }
    }
}

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

    let mut grid: Grid = Vec::new();

    for line in lines.iter() {
        grid.push(Vec::new());
        let row = grid.last_mut().unwrap();

        for c in line.chars() {
            row.push(c.to_string().parse::<i64>().unwrap());
        }
    }

    let low_points = find_low_points(&grid);
    for lp in low_points.iter() {
        println!("lp={:?}", lp);
    }
    println!();

    let mut basins = expand_basins(&grid, &low_points);

    basins.sort_by(|a, b| b.len().cmp(&a.len()));

    for basin in basins.iter() {
        println!("{}", basin.len());
    }

    let mut product: i64 = 1;
    for i in 0..3 {
        product *= basins[i].len() as i64;
    }

    println!("product={}", product);
}

fn expand_basins(grid: &Grid, low_points: &Vec<Point>) -> Vec<Basin> {
    let mut basins: Vec<Basin> = Vec::new();
    for lp in low_points.iter() {
        let mut basin: Basin = HashSet::new();
        basin_traverse(&grid, &mut basin, *lp, -1);
        basins.push(basin);
    }
    basins
}

fn basin_traverse(grid: &Grid, mut basin: &mut Basin, point: Point, height: i64) {
    if let Some(row) = grid.get(point.x) {
        if let Some(col) = row.get(point.y) {
            let me = *col;
            if me != 9 && me > height {
                basin.insert(point);

                let x_len: usize = grid.len();
                let y_len: usize = grid[0].len();

                if point.x > 0 {
                    let p = Point::new(point.x - 1, point.y);
                    basin_traverse(&grid, &mut basin, p, me);
                }

                if point.x + 1 < x_len {
                    let p = Point::new(point.x + 1, point.y);
                    basin_traverse(&grid, &mut basin, p, me);
                }

                if point.y > 0 {
                    let p = Point::new(point.x, point.y - 1);
                    basin_traverse(&grid, &mut basin, p, me);
                }

                if point.y + 1 < y_len {
                    let p = Point::new(point.x, point.y + 1);
                    basin_traverse(&grid, &mut basin, p, me);
                }
            }
        }
    }
}

fn find_low_points(grid: &Grid) -> Vec<Point> {
    let mut low_points: Vec<Point> = Vec::new();

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
                low_points.push(Point::new(x, y));
            }
        }
    }

    low_points
}
