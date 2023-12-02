use std::collections::HashMap;

use aoc_core;

#[derive(Eq, PartialEq)]
enum SpaceState
{
    Open,
    Wall,
}

static NEIGHBORS: [Pos; 4] = 
[
    Pos{ x: -1, y:  0},
    Pos{ x:  1, y:  0},
    Pos{ x:  0, y: -1},
    Pos{ x:  0, y:  1},
];

#[derive(Clone, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
struct Pos
{
    x: i64,
    y: i64,
}

impl Pos
{
    fn new(x: i64, y: i64) -> Pos
    {
        Pos { x, y}
    }

    fn add(&self, other: &Pos) -> Pos
    {
        Pos::new(self.x + other.x, self.y + other.y)
    }
}

fn calculate_space_state(p: &Pos, favorite: i64) -> SpaceState
{
    let x = p.x;
    let y = p.y;

    let v = x*x + 3*x + 2*x*y + y + y*y + favorite;
    let b = format!("{v:b}");
    let ones = b.chars().fold(0, |a, c| if c == '1' { a + 1 } else { a });

    if ones % 2 == 0
    {
        return SpaceState::Open;
    }
    return SpaceState::Wall;
}

fn search(c: Pos, d: i64, favorite: i64, visited: &mut HashMap<Pos, i64>)
{
    // If its in bounds x >= 0 and y >= 0
    // If its not a wall
    // If we are within the step coutn
    if c.x >= 0 && c.y >= 0 && d <= 50 && calculate_space_state(&c, favorite) == SpaceState::Open
    {
        // If the previous traversal is the same or less then that path has explored as much or more, stop.
        if let Some(pd)= visited.get(&c)
        {
            if *pd <= d
            {
                return;
            }
        }

        // Insert or update the node since its new or a shorter path.
        *visited.entry(c.clone()).or_insert(0) = d;

        // Traverse all neighbors.
        for n in NEIGHBORS.iter()
        {
            search(c.add(n), d + 1, favorite, visited);
        }
    }
}

fn main() {
    let lines = aoc_core::input_to_lines();
    let favorite = lines[0].parse::<i64>().unwrap();

    let mut visited = HashMap::new();
    search(Pos::new(1, 1), 0, favorite, &mut visited);
    println!("{}", visited.len());
}
