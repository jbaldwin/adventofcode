use aoc_core;

#[derive(Clone, PartialEq)]
struct Point {
    x: i64,
    y: i64,
}

impl Point {
    fn new(x: i64, y: i64) -> Point {
        Point { x, y }
    }
}

impl std::fmt::Display for Point {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "{},{}", self.x, self.y)
    }
}

static START: Point = Point{ x: 500, y: 0 };

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');

    let mut rocks = Vec::new();

    for line in lines.iter() {
        let mut rock_points = Vec::new();
        let parts = aoc_core::str_split_str(line, " -> ");
        for part in parts.iter() {
            let point_parts = aoc_core::str_split_char(part, ',');
            rock_points.push(
                Point::new(
                    point_parts.get(0).unwrap().parse::<i64>().unwrap(),
                    point_parts.get(1).unwrap().parse::<i64>().unwrap(),
                ));
        }
        rocks.push(rock_points);
    }

    let mut print_min_x = i64::MAX as usize;
    let min_x = 0;
    let mut max_x = usize::MIN;
    let min_y = 0usize;
    let mut max_y = usize::MIN;

    for rock in rocks.iter() {
        for point in rock.iter() {
            print_min_x = std::cmp::min(print_min_x.try_into().unwrap(), point.x).try_into().unwrap();
            // min_x = std::min(min_x, point.x);
            max_x = std::cmp::max(max_x.try_into().unwrap(), point.x).try_into().unwrap();

            // min_y = std::min(min_y, point.y);
            max_y = std::cmp::max(max_y.try_into().unwrap(), point.y).try_into().unwrap();
        }
    }

    print_min_x = 0;
    max_x *= 4; // add some buffer to this side
    max_y += 3; // need to account for the floor

    let mut cave = Vec::new();
    for _y in min_y..=max_y {
        cave.push(Vec::new());
        let last = cave.last_mut().unwrap();
        for _x in min_x..=max_x {
            last.push('.');
        }
    }

    for rock in rocks.iter() {
        let mut prev: Option<&Point> = None;
        for p2 in rock.iter() {
            match prev {
                Some(p1) => {
                    let mut curr = p2.clone();
                    let dir = if p1.x == p2.x {
                        // Go down
                        if p1.y > p2.y {
                            Point::new(0, 1)
                        }
                        // Go up
                        else {
                            Point::new(0, -1)
                        }
                    }
                    else {
                        // Go left
                        if p1.x > p2.x {
                            Point::new(1, 0)
                        }
                        // Go right
                        else {
                            Point::new(-1, 0)
                        }
                    };

                    while curr != *p1 {
                        cave[curr.y as usize][curr.x as usize] = '#';
                        curr.x += dir.x;
                        curr.y += dir.y;
                    }

                    prev = Some(p2);
                },
                None => {
                    cave[p2.y as usize][p2.x as usize] = '#';
                    prev = Some(p2);
                }
            }
        }
    }

    // add the floor in
    let floor = cave.get_mut((max_y - 1) as usize).unwrap();
    for x in 0..=max_x {
        floor[x] = '#';
    }

    'outer: loop {
        let mut sand = START.clone();

        loop {
            if cave[(sand.y + 1) as usize][sand.x as usize] == '.' {
                sand.y += 1;
            }
            else if cave[(sand.y + 1) as usize][(sand.x - 1) as usize] == '.' {
                sand.y += 1;
                sand.x -= 1;
            }
            else if cave[(sand.y + 1) as usize][(sand.x + 1) as usize] == '.' {
                sand.y += 1;
                sand.x += 1;
            }
            else {
                cave[sand.y as usize][sand.x as usize] = 'O';
                if sand == START {
                    break 'outer;
                }
                break;
            }
        }
    }

    let mut sand_count: usize = 0;
    for y in min_x..=max_y {
        for x in print_min_x..=max_x {
            if cave[y][x] == 'O' {
                sand_count += 1;
            }
        }
    }

    println!("{}", sand_count);
}
