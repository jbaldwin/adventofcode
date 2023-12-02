use pathfinding::prelude::astar;

use aoc_core;

#[derive(Eq, PartialEq)]
enum SpaceState
{
    Open,
    Wall,
}

static NEIGHBORS: [Pos; 4] = 
[
    Pos{ x: -1, y:  0 },
    Pos{ x:  1, y:  0 },
    Pos{ x:  0, y: -1 },
    Pos{ x:  0, y:  1 },
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
        Pos { x, y }
    }

    fn successors(&self, favorite: i64) -> Vec<(Pos, i64)>
    {
        let mut s = Vec::new();

        for n in NEIGHBORS.iter()
        {
            let c = self.add(&n);

            // If the space is in bounds and isn't a wall include it as a neighbor.
            if c.x >= 0 && c.y >= 0 && calculate_space_state(c.x, c.y, favorite) == SpaceState::Open
            {
                s.push((c, 1));
            }
        }

        s
    }

    fn distance(&self, other: &Pos) -> i64
    {
        (self.x - other.x).abs() + (self.y - other.y).abs()
    }

    fn add(&self, other: &Pos) -> Pos
    {
        Pos::new(self.x + other.x, self.y + other.y)
    }
}

fn calculate_space_state(x: i64, y: i64, favorite: i64) -> SpaceState
{
    let v = x*x + 3*x + 2*x*y + y + y*y + favorite;
    let b = format!("{v:b}");
    let ones = b.chars().fold(0, |a, c| if c == '1' { a + 1 } else { a });

    if ones % 2 == 0
    {
        return SpaceState::Open;
    }
    return SpaceState::Wall;
}

fn main() {
    let lines = aoc_core::input_to_lines();
    let favorite = lines[0].parse::<i64>().unwrap();
    let goal_coords = aoc_core::str_split_char(lines[1].as_str(), ',');
    let goal = Pos::new(goal_coords[0].parse::<i64>().unwrap(), goal_coords[1].parse::<i64>().unwrap());

    let (_, distance) = astar(
        &Pos::new(1, 1), 
        |p| p.successors(favorite),
        |p| p.distance(&goal),
        |p| *p == goal).unwrap();

    
    println!("{distance}");
}
