use aoc_core;

struct Box {
    x1: i64,
    x2: i64,
    y1: i64,
    y2: i64,
}

impl Box {
    fn intersects(&self, p: &Point) -> bool {
        p.x >= self.x1 && p.x <= self.x2 && 
        p.y >= self.y1 && p.y <= self.y2
    }

    fn past(&self, p: &Point) -> bool {
        // y1 is always "less" than y2 in the input and it is negative
        p.x > self.x2 || p.y < self.y1
    }
}

#[derive(Clone)]
struct Point {
    x: i64,
    y: i64,
}


impl std::fmt::Display for Point {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "[{}, {}]", self.x, self.y)
    }
}

fn launch_probe(target: &Box, mut vel: Point) -> bool {
    let mut y_max: i64 = 0;
    let mut pos = Point { x: 0, y: 0 };

    loop {
        pos.x += vel.x;
        pos.y += vel.y;

        if vel.x > 0 {
            vel.x -= 1;
        }
        else if vel.x < 0 {
            vel.x += 1;
        }
        vel.y -= 1;

        if pos.y > y_max {
            y_max = pos.y;
        }

        if target.intersects(&pos) {
            return true;
        }

        if target.past(&pos) {
            return false;
        }
    }
}


fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();

    let stripped = &contents[13..];

    let parts = aoc_core::str_split_str(stripped, ", ");
    let x_str = &parts[0][2..];
    let y_str = &parts[1][2..];

    let x_parts = aoc_core::str_split_str(x_str, "..");
    let y_parts = aoc_core::str_split_str(y_str, "..");

    let target = Box {
        x1: x_parts[0].parse::<i64>().unwrap(),
        x2: x_parts[1].parse::<i64>().unwrap(),
        y1: y_parts[0].parse::<i64>().unwrap(),
        y2: y_parts[1].parse::<i64>().unwrap(),
    };

    // Set some bounds on what we will try, anything past the target cannot be in the solution set.
    // The issue is how high on y do you go? -target.y1 appears to work for both example and input.
    let x_stop = target.x2;
    let y_neg = target.y1;
    let y_pos = -target.y1;

    let mut valid = Vec::new();

    for x in 0..=x_stop {
        for y in y_neg..=y_pos {
            let vel = Point { x, y };

            if launch_probe(&target, vel.clone()) {
                valid.push(vel);
            }
        }
    }

    println!("{}", valid.len());
}
