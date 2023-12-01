use aoc_core;

#[derive(Clone)]
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
        write!(f, "{}, {}", self.x, self.y)
    }
}

enum Jet {
    Left,
    Right
}

enum Shapes {
    HorizontalLine,
    Star,
    Elbow,
    VerticalLine,
    Square,
}

struct Shape {
    points: Vec<Point>,
}

impl Shape {
    fn new_horizontal_line() -> Shape {
        Shape {
            points: vec![
                Point::new(0, 0),
                Point::new(1, 0),
                Point::new(2, 0),
                Point::new(3, 0),
            ],
        }
    }

    fn new_star() -> Shape {
        Shape {
            points: vec![
                Point::new(1, 0),
                Point::new(0, 1),
                Point::new(1, 1),
                Point::new(2, 1),
                Point::new(1, 2),
            ],
        }
    }

    fn new_elbow() -> Shape {
        Shape {
            points: vec![
                Point::new(0, 0),
                Point::new(1, 0),
                Point::new(2, 0),
                Point::new(2, 1),
                Point::new(2, 2),
            ],
        }
    }

    fn new_vertical_line() -> Shape {
        Shape {
            points: vec![
                Point::new(0, 0),
                Point::new(0, 1),
                Point::new(0, 2),
                Point::new(0, 3),
            ],
        }
    }

    fn square() -> Shape {
        Shape {
            points: vec![
                Point::new(0, 0),
                Point::new(1, 0),
                Point::new(0, 1),
                Point::new(1, 1),
            ]
        }
    }

    fn push(&mut self, x: i64, y: i64) {
        for p in self.points.iter_mut() {
            p.x += x;
            p.y += y;
        }
    }

    fn max_y(&self) -> i64 {
        let mut max_y = i64::MIN;
        for p in self.points.iter() {
            max_y = std::cmp::max(max_y, p.y);
        }
        max_y
    }

    fn hit_wall_or_rock(&self, chamber: &Vec<[char; 9]>) -> bool {
        for p in self.points.iter() {
            match chamber[p.y as usize][p.x as usize] {
                '|' | '#' => return true,
                _ => { },
            };
        }
        false
    }

    fn hit_rock(&self, chamber: &Vec<[char; 9]>) -> bool {
        for p in self.points.iter() {
            // Count rocks or the floor as rock.
            match chamber[p.y as usize][p.x as usize] {
                '#' | '-' => {
                    // We've hit rock, cement in place.
                    return true;
                },
                _ => { },
            }
        }
        false
    }
}

impl std::fmt::Display for Shape {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        for p in self.points.iter() {
            let r = write!(f, "[{}],", p);
            if r.is_err() {
                return r;
            }
        }
        std::fmt::Result::Ok(())
    }
}

fn add_row(chamber: &mut Vec<[char; 9]>) {
    chamber.push(['|', '.', '.', '.', '.', '.', '.', '.', '|']);
}

fn print_chamber(chamber: &Vec<[char; 9]>) {
    for r in chamber.iter().rev() {
        println!("{}{}{}{}{}{}{}{}{}", r[0],r[1],r[2],r[3],r[4],r[5],r[6],r[7],r[8]);
    }
}

/// Used for debugging.
// fn print_chamber_shape(chamber: &Vec<[char; 9]>, shape: &Shape) {
//     for (mut y, r) in chamber.iter().rev().enumerate() {
//         y = chamber.len() - y - 1;
//         print!("{}", r[0]);
//         for x  in 1..(r.len() - 1) as i64 {
//             let mut printed = false;
//             for p in shape.points.iter() {
//                 if p.x == x && p.y == y as i64 {
//                     print!("@");
//                     printed = true;
//                     break;
//                 }
//             }

//             if !printed {
//                 print!("{}", r[x as usize]);
//             }

//         }
//         println!("{}", r[8]);
//     }
// }

fn find_highest_rock(chamber: &Vec<[char; 9]>) -> i64 {
    for (mut y, r) in chamber.iter().rev().enumerate() {
        y = chamber.len() - y - 1;
        for c in r {
            if *c == '#' || *c == '-' {
                return y as i64;
            }
        }
    }
    panic!("no rock found");
}

static START_X: i64 = 3; // account for the wall
static START_Y: i64 = 4;

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();

    let mut jet_pattern = Vec::with_capacity(contents.len());
    let shape_order = [
        Shapes::HorizontalLine,
        Shapes::Star,
        Shapes::Elbow,
        Shapes::VerticalLine,
        Shapes::Square,
    ];

    for c in contents.chars() {
        match c {
            '<' => jet_pattern.push(Jet::Left),
            '>' => jet_pattern.push(Jet::Right),
            _ => panic!("invalid input {}", c),
        };
    }

    let mut chamber = Vec::with_capacity(contents.len());
    chamber.push(['+', '-', '-', '-', '-', '-', '-', '-', '+']);

    let mut j = 0usize;
    for i in 0..2022 {

        let y_highest_rock = find_highest_rock(&chamber);

        let shape_type = &shape_order[(i % shape_order.len()) as usize];
        let mut shape = match shape_type {
            Shapes::HorizontalLine => Shape::new_horizontal_line(),
            Shapes::Star => Shape::new_star(),
            Shapes::Elbow => Shape::new_elbow(),
            Shapes::VerticalLine => Shape::new_vertical_line(),
            Shapes::Square => Shape::square(),
        };

        // adjust the starting position
        shape.push(START_X, START_Y + y_highest_rock);
        let shape_max_y = shape.max_y();
        for _ in chamber.len()..=(shape_max_y as usize) {
            add_row(&mut chamber);
        }

        // While we haven't hit rock, jet move, then gravity move.
        loop {
            let jet = &jet_pattern[(j % jet_pattern.len()) as usize];
            j += 1;
            let x_move: i64 = match jet {
                Jet::Left => -1,
                Jet::Right => 1,
            };

            shape.push(x_move, 0);
            if shape.hit_wall_or_rock(&chamber) {
                // undo if we hit a wall
                shape.push(-x_move, 0);
            }

            shape.push(0, -1);
            if shape.hit_rock(&chamber) {
                shape.push(0, 1);

                // cememt in chamber
                for p in shape.points.iter() {
                    chamber[p.y as usize][p.x as usize] = '#';
                }

                break;
            }
        }
    }
    print_chamber(&chamber);
    println!();

    let y = find_highest_rock(&chamber);
    println!("{}", y);
    
}
