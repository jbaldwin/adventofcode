use aoc_core;

use std::collections::{HashSet, VecDeque};

#[derive(Clone, Debug, Eq, Hash, Ord, PartialEq, PartialOrd)]
struct Point
{
    x: usize,
    y: usize,
}

fn find_start(diagram: &Vec<Vec<char>>) -> Point
{
    for i in 0..diagram.len()
    {
        for j in 0..diagram[0].len()
        {
            if diagram[i][j] == 'S'
            {
                return Point { x: i, y: j };
            }
        }
    }

    panic!("no starting point");
}

fn determine_s_pipe(s: &Point, diagram: &mut Vec<Vec<char>>)
{
    // North
    let mut north = false;
    {
        let p1 = Point { x: s.x - 1, y: s.y };
        if let Some(row) = diagram.get(p1.x)
        {
            if let Some(c) = row.get(p1.y)
            {
                if *c == '|' || *c == 'F' || *c == '7'
                {
                    north = true;
                }
            }
        }
    }

    // South
    let mut south = false;
    {
        let p1 = Point { x: s.x + 1, y: s.y };
        if let Some(row) = diagram.get(p1.x)
        {
            if let Some(c) = row.get(p1.y)
            {
                if *c == '|' || *c == 'L' || *c == 'J'
                {
                    south = true;
                }
            }
        }
    }

    // East
    let mut east = false;
    {
        let p1 = Point { x: s.x, y: s.y + 1 };
        if let Some(row) = diagram.get(p1.x)
        {
            if let Some(c) = row.get(p1.y)
            {
                if *c == '-' || *c == 'J' || *c == '7'
                {
                    east = true;
                }
            }
        }
    }

    // West
    let mut west = false;
    {
        let p1 = Point { x: s.x, y: s.y - 1 };
        if let Some(row) = diagram.get(p1.x)
        {
            if let Some(c) = row.get(p1.y)
            {
                if *c == '-' || *c == 'L' || *c == 'F'
                {
                    west = true;
                }
            }
        }
    }

    if north && east
    {
        diagram[s.x][s.y] = 'L';
    }
    else if north && south
    {
        diagram[s.x][s.y] = '|';
    }
    else if north && west
    {
        diagram[s.x][s.y] = 'J';
    }
    else if east && south
    {
        diagram[s.x][s.y] = 'F';
    }
    else if east && west
    {
        diagram[s.x][s.y] = '-';
    }
    else if south && west
    {
        diagram[s.x][s.y] = '7';
    }
    else
    {
        panic!("could not determine what S should be");
    }

}

enum Direction
{
    North,
    East,
    South,
    West,
}

fn moves(c: char) -> [Direction; 2]
{
    match c
    {
        '|' => [Direction::North, Direction::South],
        '-' => [Direction::East, Direction::West],
        'L' => [Direction::North, Direction::East],
        'J' => [Direction::North, Direction::West],
        '7' => [Direction::South, Direction::West],
        'F' => [Direction::South, Direction::East],
        _ => panic!("invalid char"),
    }
}

fn traverse(c: Point, diagram: &Vec<Vec<char>>, visited: &mut HashSet<Point>, connected_right: &mut HashSet<Point>)
{
    // Do a BFS since we need to calculate steps outwards from the starting position.
    let mut q = VecDeque::new();
    q.push_back((c, 0));

    while let Some((point, d)) = q.pop_front()
    {
        if let Some(row) = diagram.get(point.x)
        {
            if let Some(col) = row.get(point.y)
            {
                if visited.insert(point.clone())
                {
                    match *col {
                        '-' | '7' | 'L' => connected_right.insert(point.clone()),
                        _ => { false },
                    };

                    let ms = moves(*col);
                    for m in ms
                    {
                        match m
                        {
                            Direction::North => {
                                q.push_back((Point { x: point.x - 1, y: point.y }, d + 1));
                            },
                            Direction::South => {
                                q.push_back((Point { x: point.x + 1, y: point.y }, d + 1));
                            },
                            Direction::East => {
                                q.push_back((Point { x: point.x, y: point.y + 1 }, d + 1));
                            },
                            Direction::West => {
                                q.push_back((Point { x: point.x, y: point.y - 1 }, d + 1));
                            },
                        }
                    }
                }
            }
        }
    }
}

fn main()
{
    let lines = aoc_core::input_to_lines();

    let mut diagram = Vec::new();
    for line in lines.iter()
    {
        let mut row = Vec::new();
        for c in line.chars()
        {
            row.push(c);
        }
        diagram.push(row);
    }

    // Update the S point to be a normal pipe for collision detection.
    let s = find_start(&diagram);
    determine_s_pipe(&s, &mut diagram);

    let mut in_loop = HashSet::new();
    let mut connected_right = HashSet::new();
    traverse(s, &diagram, &mut in_loop, &mut connected_right);

    let mut sum = 0;
    for (x, row) in diagram.iter().enumerate()
    {
        for (y, _) in row.iter().enumerate()
        {
            if in_loop.contains(&Point { x, y })
            {
                continue;
            }

            if x == 0 || y == 0 || x + 1 == diagram.len() || y + 1 == diagram[0].len()
            {
                continue;
            }

            let mut crosses = [0, 0, 0, 0];

            let ranges = [(0, x, "X"), (x + 1, diagram.len(), "X"), (0, y, "Y"), (y + 1, diagram[0].len(), "Y")];

            for (range_idx, (start, end, axis)) in ranges.iter().enumerate()
            {
                for n in *start..*end
                {
                    if *axis == "X"
                    {
                        if in_loop.contains(&Point { x: n, y })
                        {
                            crosses[range_idx] += 1;
                        }
                    }
                    else
                    {
                        if in_loop.contains(&Point { x, y: n })
                        {
                            crosses[range_idx] += 1;
                        }
                    }
                }
            }

            // If any direction doesn't cross a pipe then its outside.
            if crosses.iter().any(|n| *n == 0)
            {
                continue;
            }

            let mut right_crosses = 0;
            for n in (0..y).rev()
            {
                if connected_right.contains(&Point { x, y: n })
                {
                    right_crosses += 1;
                }
            }

            // if (crosses[0] % 2 == 1 && crosses[1] % 2 == 1) || (crosses[2] % 2 == 1 && crosses[3] % 2 == 1)
            if right_crosses % 2 == 1
            {
                sum += 1;
                println!();
                println!("{}", right_crosses);
                // println!("{}, {}, {}, {}, {}", crosses[0], crosses[0], crosses[0], crosses[0], right_crosses);
                for (i, row) in diagram.iter().enumerate()
                {
                    for (j, col) in row.iter().enumerate()
                    {
                        if i == x && j == y
                        {
                            print!("X");
                        }
                        else
                        {
                            print!("{}", col);
                        }
                    }
                    println!();
                }
            }
        }
    }
    println!("{}", sum);
}
