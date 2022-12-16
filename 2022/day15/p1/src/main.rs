use aoc_core;
use std::time::Instant;

#[derive(Eq, PartialEq, Hash, Clone)]
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
        write!(f, "x={}, y={}", self.x, self.y)
    }
}

struct Sensor {
    s: Point,
    b: Point,
    md: i64,
}

impl Sensor {
    fn new(s: Point, b: Point) -> Sensor {
        let md = (s.x - b.x).abs() + (s.y - b.y).abs();

        Sensor {
            s,
            b,
            md,
        }
    }
}

fn main() {
    let now = Instant::now();
    let args = aoc_core::args_init(3);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');

    let y = args[2].parse::<i64>().unwrap();

    let mut arrangement = Vec::new();

    let mut min_x = i64::MAX;
    let mut max_x = i64::MIN;

    for line in lines.iter() {
        let s_b_parts = aoc_core::str_split(line, ": ");
        let sensor_str = &s_b_parts[0][10..];
        let beacon_str = &s_b_parts[1][21..];

        let s_parts = aoc_core::str_split(sensor_str, ", ");
        let b_parts = aoc_core::str_split(beacon_str, ", ");

        let s_x_part = &s_parts[0][2..];
        let s_y_part = &s_parts[1][2..];

        let b_x_part = &b_parts[0][2..];
        let b_y_part = &b_parts[1][2..];

        let s = Point::new(s_x_part.parse::<i64>().unwrap(), s_y_part.parse::<i64>().unwrap());
        let b = Point::new(b_x_part.parse::<i64>().unwrap(), b_y_part.parse::<i64>().unwrap());

        let sensor = Sensor::new(s.clone(), b);

        min_x = std::cmp::min(min_x, s.x - sensor.md);
        max_x = std::cmp::max(max_x, s.x + sensor.md);

        arrangement.push(sensor);
    }

    let mut searched = 0usize;

    let capacity = (max_x + min_x.abs()) as usize;
    let mut y_row = Vec::with_capacity(capacity);

    for _ in min_x..=max_x {
        y_row.push(false);
    }
    let min_x_abs = min_x.abs();

    // Prepopulate beacons as 'searched' but don't count towards the total.
    for sensor in arrangement.iter() {
        if sensor.b.y == y {
            y_row[(sensor.b.x + min_x_abs) as usize] = true;
        }
    }
    let elapsed = now.elapsed();
    println!("setup {}ms", elapsed.as_millis());

    let now = Instant::now();
    for sensor in arrangement.iter() {
        let s = &sensor.s;
        let y_dist = (s.y - y).abs();

        // This sensor can never reach the Y row.
        if y_dist > sensor.md {
            continue;
        }

        let x_remaining_dist = sensor.md - y_dist;

        // Start at the intersection point and go up and down until no matches
        for x in s.x..=(s.x + x_remaining_dist) {
            if y_row[(x + min_x_abs) as usize] == false {
                searched += 1;
                y_row[(x + min_x_abs) as usize] = true;
            }
        }

        for x in (s.x - x_remaining_dist)..s.x {
            if y_row[(x + min_x_abs) as usize] == false {
                searched += 1;
                y_row[(x + min_x_abs) as usize] = true;
            }
        }
    }
    let elapsed = now.elapsed();

    println!("{} in {}ms", searched, elapsed.as_millis());
}
