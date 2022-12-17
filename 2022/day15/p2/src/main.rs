use aoc_core;
use std::cmp::Ordering;
use std::collections::HashSet;
use std::process::exit;
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

    fn overlaps_or_touching(&self, p2: &Point) -> bool {
        p2.x <= self.y + 1
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

    let iterations = args[2].parse::<i64>().unwrap();

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

    let elapsed = now.elapsed();
    println!("setup {}ms", elapsed.as_millis());

    let mut ranges = Vec::with_capacity(arrangement.len());

    let now = Instant::now();
    for y in 0..=iterations {
        ranges.clear();

        for sensor in arrangement.iter() {
            let s = &sensor.s;
            let y_dist = (s.y - y).abs();
    
            // This sensor can never reach the Y row.
            if y_dist > sensor.md {
                continue;
            }
    
            let x_remaining_dist = sensor.md - y_dist;
            ranges.push(Point::new(s.x - x_remaining_dist, s.x + x_remaining_dist));
        }

        // merge ranges
        ranges.sort_by(|p1, p2| {
            if p1.x < p2.x {
                Ordering::Less
            }
            else if p1.x == p2.x {
                if p1.y == p2.y {
                    Ordering::Equal
                } else if p1.y < p2.y {
                    Ordering::Less
                } else {
                    Ordering::Greater
                }
            } else {
                Ordering::Greater
            }
        });

        let mut i = 0usize;
        while i < ranges.len() - 1 {
            let mut combine = false;
            let mut new_y = 0i64;
            {
                let p1 = &ranges[i];
                let p2 = &ranges[i + 1];

                // combine if they intersect and it would extend it
                if p1.overlaps_or_touching(p2) {
                    combine = true;
                    new_y = std::cmp::max(p1.y, p2.y);
                }
            }

            if combine {
                ranges.get_mut(i).unwrap().y = new_y;
                ranges.remove(i + 1);
            } else {
                i += 1;
            }
        }

        if ranges.len() > 1 {
            for range in ranges.iter() {
                print!("[{}]", range);
            }
            let distress_beacon = Point::new(ranges.first().unwrap().y + 1, y);

            println!("\n[{}]", distress_beacon);
            println!("{}", distress_beacon.x * 4000000 + y);

            let elapsed = now.elapsed();
            println!("\n{}ms", (elapsed.as_micros() as f64) / 1000f64);
            exit(0);
        }
    }
}
