use aoc_core;

#[derive(Clone, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
struct Point {
    x: usize,
    y: usize,
}

impl Point {
    fn new(x: usize, y: usize) -> Point {
        Point { x, y }
    }

    fn successors(&self, heightmap: &Vec<Vec<u32>>) -> Vec<Point> {
        let mut successors = Vec::new();

        let my_height = *heightmap.get(self.x).unwrap().get(self.y).unwrap();

        if self.x > 0 {
            let p = Point::new(self.x - 1, self.y);
            let new_height = *heightmap.get(p.x).unwrap().get(p.y).unwrap();
            if new_height <= my_height + 1 {
                successors.push(p);
            }
        }

        if self.x < heightmap.len() - 1 {
            let p = Point::new(self.x + 1, self.y);
            let new_height = *heightmap.get(p.x).unwrap().get(p.y).unwrap();
            if new_height <= my_height + 1 {
                successors.push(p);
            }
        }

        if self.y > 0 {
            let p = Point::new(self.x, self.y - 1);
            let new_height = *heightmap.get(p.x).unwrap().get(p.y).unwrap();
            if new_height <= my_height + 1 {
                successors.push(p);
            }
        }

        if self.y < heightmap.get(0).unwrap().len() - 1 {
            let p = Point::new(self.x, self.y + 1);
            let new_height = *heightmap.get(p.x).unwrap().get(p.y).unwrap();
            if new_height <= my_height + 1 {
                successors.push(p);
            }
        }

        successors
    }
}

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');

    let mut s = Point::new(0, 0);
    let mut e = Point::new(0, 0);

    let mut visited = Vec::new();
    let mut heightmap = Vec::new();

    let mut x: usize = 0;
    for line in lines.iter() {
        visited.push(Vec::new());
        let visited_last = visited.last_mut().unwrap();
        heightmap.push(Vec::new());
        let heightmap_last = heightmap.last_mut().unwrap();

        let mut y: usize = 0;
        for c in line.chars() {
            if c == 'S' {
                heightmap_last.push(0);
                s = Point::new(x, y);
            } else if c == 'E' {
                heightmap_last.push(25);
                e = Point::new(x, y);
            } else {
                heightmap_last.push(c as u32 - 97);
            }
            visited_last.push(false);
            y += 1;
        }

        x += 1;
    }

    let result = pathfinding::prelude::bfs(
        &s,
        |p| p.successors(&heightmap),
        |p| *p == e
    ).unwrap().len() - 1;

    println!("{}", result);
}
