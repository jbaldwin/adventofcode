use aoc_core;
use std::collections::HashSet;
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
    md: i64,
}

impl Sensor {
    fn new(s: Point, b: &Point) -> Sensor {
        let md = (s.x - b.x).abs() + (s.y - b.y).abs();

        Sensor {
            s,
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
    let mut beacons = HashSet::new();

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

        let sensor = Sensor::new(s.clone(), &b);

        beacons.insert(b);

        min_x = std::cmp::min(min_x, s.x - sensor.md);
        max_x = std::cmp::max(max_x, s.x + sensor.md);

        arrangement.push(sensor);
    }

    let capacity = (max_x + min_x.abs()) as usize;
    let mut y_row = Vec::with_capacity(capacity);

    for _ in min_x..=max_x {
        y_row.push(false);
    }
    let min_x_abs = min_x.abs();

    let elapsed = now.elapsed();
    println!("setup {}ms", elapsed.as_millis());

    let now = Instant::now();
    // Prepopulate beacons as 'searched' but don't count towards the total.
    let mut beacons_in_row = 0i64;
    for beacon in beacons.iter() {
        if beacon.y == y {
            y_row[(beacon.x + min_x_abs) as usize] = true;
            beacons_in_row += 1;
        }
    }

    for sensor in arrangement.iter() {
        let s = &sensor.s;
        let y_dist = (s.y - y).abs();

        // This sensor can never reach the Y row.
        if y_dist > sensor.md {
            continue;
        }

        let x_remaining_dist = sensor.md - y_dist;

        // Start at the intersection point and go left and right for remaining x dist.
        for x in (s.x + min_x_abs)..=(s.x + x_remaining_dist + min_x_abs) {
            y_row[x as usize] = true;
        }

        for x in (s.x - x_remaining_dist + min_x_abs)..(s.x + min_x_abs) {
            y_row[x as usize] = true;
        }
    }
    let elapsed = now.elapsed();

    let total: i64 = y_row.iter().fold(0, |a, b| {
        match *b {
            true => a + 1,
            false => a,
        }
    });

    println!("{} in {}ms", total - beacons_in_row, elapsed.as_millis());
}
