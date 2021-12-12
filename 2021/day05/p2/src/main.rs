use std::collections::HashMap;

#[derive(Eq, Hash)]
struct Point {
    x: i64,
    y: i64,
}

impl Point {
    fn new(x: i64, y: i64) -> Point {
        Point { x, y }
    }
}

impl PartialEq for Point {
    fn eq(&self, other: &Self) -> bool {
        self.x == other.x && self.y == other.y
    }
}

impl std::fmt::Display for Point {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "{},{}", self.x, self.y)
    }
}

struct Line {
    a: Point,
    b: Point,
}

impl Line {
    fn new(s: &str) -> Line {
        let mut parts = s.split(" -> ");
        let a_str = parts.next().unwrap();
        let b_str = parts.next().unwrap();

        let mut a_split = a_str.split(',');
        let mut b_split = b_str.split(',');

        let a_x = a_split.next().unwrap();
        let a_y = a_split.next().unwrap();

        let b_x = b_split.next().unwrap();
        let b_y = b_split.next().unwrap();
        
        Line { 
            a: Point::new(
                a_x.parse::<i64>().unwrap(),
                a_y.parse::<i64>().unwrap()
            ),
            b: Point::new(
                b_x.parse::<i64>().unwrap(),
                b_y.parse::<i64>().unwrap()
            ) 
        }
    }

    fn draw(&self) -> Vec<Point> {
        let mut points: Vec<Point> = Vec::new();

        let mut x_step: i64 = 0;
        let mut y_step: i64 = 0;

        if self.a.x != self.b.x {
            x_step = if self.a.x < self.b.x { 1 } else { -1 };
        }

        if self.a.y != self.b.y { 
            y_step = if self.a.y < self.b.y { 1 } else { -1 };
        }

        let mut x = self.a.x;
        let mut y = self.a.y;

        // Walk until the points meet.
        while x != self.b.x || y != self.b.y {
            points.push(Point::new(x, y));
            x = x + x_step;
            y = y + y_step;
        }
        points.push(Point::new(x, y)); // Add the final point

        points
    }
}

impl std::fmt::Display for Line {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "{} -> {}", self.a, self.b)
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

    let mut vents: Vec<Line> = Vec::new();

    for line in lines.iter() {
        vents.push(Line::new(line));
    }

    let mut data: HashMap<Point, i64> = HashMap::new();

    for v in vents.iter() {
        let line_points = v.draw();
        for point in line_points {
            if let Some(p) = data.get_mut(&point) {
                *p = *p + 1;
            }
            else
            {
                data.insert(point, 1);
            }
        }
    }

    let mut count: i64 = 0;
    for p in data {
        if p.1 >= 2 {
            count = count + 1;
        }
    }

    println!("{}", count);
}
