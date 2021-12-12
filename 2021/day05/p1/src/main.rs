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
            a: Point::new(a_x.parse::<i64>().unwrap(), a_y.parse::<i64>().unwrap()),
            b: Point::new(b_x.parse::<i64>().unwrap(), b_y.parse::<i64>().unwrap()),
        }
    }

    fn is_horizontal_or_vertical(&self) -> bool {
        if self.a.x == self.b.x || self.a.y == self.b.y {
            return true;
        }
        return false;
    }

    fn draw(&self) -> Vec<Point> {
        let mut points: Vec<Point> = Vec::new();

        if self.a.x != self.b.x {
            let y = self.a.y;
            let mut x_start = std::cmp::min(self.a.x, self.b.x);
            let x_stop = std::cmp::max(self.a.x, self.b.x);

            while x_start != x_stop {
                points.push(Point::new(x_start, y));
                x_start = x_start + 1;
            }
            points.push(Point::new(x_start, y));
        } else {
            let x = self.a.x;
            let mut y_start = std::cmp::min(self.a.y, self.b.y);
            let y_stop = std::cmp::max(self.a.y, self.b.y);

            while y_start != y_stop {
                points.push(Point::new(x, y_start));
                y_start = y_start + 1;
            }
            points.push(Point::new(x, y_start));
        }

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
        let l = Line::new(line);
        if l.is_horizontal_or_vertical() {
            vents.push(l);
        }
    }

    let mut data: HashMap<Point, i64> = HashMap::new();

    for v in vents.iter() {
        let line_points = v.draw();
        for point in line_points {
            if let Some(p) = data.get_mut(&point) {
                *p = *p + 1;
            } else {
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
