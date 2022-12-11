use aoc_core;
use std::collections::HashSet;

#[derive(Eq, Hash, Clone)]
struct Point {
    x: i32,
    y: i32,
}

impl Point {
    fn new(x: i32, y: i32) -> Point {
        Point {
            x,
            y
        }
    }
}

impl PartialEq for Point {
    fn eq(&self, other: &Self) -> bool {
        self.x == other.x && self.y == other.y
    }
}

impl std::fmt::Display for Point {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        write!(f, "[{}, {}]", self.x, self.y)
    }
}

fn pull_rope(rope: &mut Vec<Point>, visited: &mut HashSet<Point>) {
    for i in 0..(rope.len() - 1) {
        let cy = rope[i].y;
        let cx = rope[i].x;
        let ny = rope[i + 1].y;
        let nx = rope[i + 1].x;

        let y_diff = cy - ny;
        let x_diff = cx - nx;

        // If within -1..=1 for both x and y they are adjacent, do not move.
        if y_diff >= -1 && y_diff <= 1 && x_diff >= -1 && x_diff <= 1 {
            continue;
        }

        //  y, x
        //  2, 0,
        //  2,-1,
        //  2,-2,
        //  1,-2,
        //  0,-2,
        // -1,-2,
        // -2,-2,
        // -2,-1,
        // -2, 0,
        // -2, 1,
        // -2, 2,
        // -1, 2,
        //  0, 2,
        //  1, 2,
        //  2, 2,
        //  2, 1

        //     x
        // ..........
        // ..........
        // ..........
        // ..HHHHH...
        // ..H,,,H...
        // ..H,T,H...y
        // ..H,,,H...
        // ..HHHHH...
        // ..........
        // ..........

        if y_diff == 2 && x_diff == 0 {
            rope.get_mut(i + 1).unwrap().y += 1;
            rope.get_mut(i + 1).unwrap().x += 0;
        } else if y_diff == 2 && x_diff == -1 {
            rope.get_mut(i + 1).unwrap().y += 1;
            rope.get_mut(i + 1).unwrap().x += -1;
        } else if y_diff == 2 && x_diff == -2 {
            rope.get_mut(i + 1).unwrap().y += 1;
            rope.get_mut(i + 1).unwrap().x += -1;
        } else if y_diff == 1 && x_diff == -2 {
            rope.get_mut(i + 1).unwrap().y += 1;
            rope.get_mut(i + 1).unwrap().x += -1;
        } else if y_diff == 0 && x_diff == -2 {
            rope.get_mut(i + 1).unwrap().y += 0;
            rope.get_mut(i + 1).unwrap().x += -1;
        } else if y_diff == -1 && x_diff == -2 {
            rope.get_mut(i + 1).unwrap().y += -1;
            rope.get_mut(i + 1).unwrap().x += -1;
        } else if y_diff == -2 && x_diff == -2 {
            rope.get_mut(i + 1).unwrap().y += -1;
            rope.get_mut(i + 1).unwrap().x += -1;
        } else if y_diff == -2 && x_diff == -1 {
            rope.get_mut(i + 1).unwrap().y += -1;
            rope.get_mut(i + 1).unwrap().x += -1;
        } else if y_diff == -2 && x_diff == 0 {
            rope.get_mut(i + 1).unwrap().y += -1;
            rope.get_mut(i + 1).unwrap().x += 0;
        } else if y_diff == -2 && x_diff == 1 {
            rope.get_mut(i + 1).unwrap().y += -1;
            rope.get_mut(i + 1).unwrap().x += 1;
        } else if y_diff == -2 && x_diff == 2 {
            rope.get_mut(i + 1).unwrap().y += -1;
            rope.get_mut(i + 1).unwrap().x += 1;
        } else if y_diff == -1 && x_diff == 2 {
            rope.get_mut(i + 1).unwrap().y += -1;
            rope.get_mut(i + 1).unwrap().x += 1;
        } else if y_diff == 0 && x_diff == 2 {
            rope.get_mut(i + 1).unwrap().y += 0;
            rope.get_mut(i + 1).unwrap().x += 1;
        } else if y_diff == 1 && x_diff == 2 {
            rope.get_mut(i + 1).unwrap().y += 1;
            rope.get_mut(i + 1).unwrap().x += 1;
        } else if y_diff == 2 && x_diff == 2 {
            rope.get_mut(i + 1).unwrap().y += 1;
            rope.get_mut(i + 1).unwrap().x += 1;
        } else if y_diff == 2 && x_diff == 1 {
            rope.get_mut(i + 1).unwrap().y += 1;
            rope.get_mut(i + 1).unwrap().x += 1;
        } else {
            panic!("invalid position [{}, {}]", y_diff, x_diff);
        }
    }


    visited.insert(rope.last().unwrap().clone());
}

fn move_head(rope: &mut Vec<Point>, dir: char, visited: &mut HashSet<Point>) {
    {
        let mut first = rope.first_mut().unwrap();
        match dir {
            'R' => { first.x += 1 },
            'L' => { first.x -= 1 },
            'U' => { first.y += 1 },
            'D' => { first.y -= 1 },
            _ => panic!("invalid input {}", dir),
        }
    }

    pull_rope(rope, visited);
}

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');

    let mut visited = HashSet::new();
    
    let mut rope = vec![
        Point::new(0, 0),
        Point::new(0, 0),
        Point::new(0, 0),
        Point::new(0, 0),
        Point::new(0, 0),
        Point::new(0, 0),
        Point::new(0, 0),
        Point::new(0, 0),
        Point::new(0, 0),
        Point::new(0, 0),
    ];

    for line in lines.iter() {
        let parts = aoc_core::str_split_char(line, ' ');

        let amount = parts[1].parse::<usize>().unwrap();
        let dir = parts[0].chars().nth(0).unwrap();

        for _i in 0..amount {
            move_head(&mut rope, dir, &mut visited);
        }
    }

    println!("{}", visited.len());
}
