use aoc_core;

use std::collections::VecDeque;

struct Point
{
    x: usize,
    y: usize,
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

fn traverse(c: Point, diagram: &Vec<Vec<char>>, distances: &mut Vec<Vec<i32>>) -> i32
{
    // Do a BFS since we need to calculate steps outwards from the starting position.
    let mut q = VecDeque::new();
    q.push_back((c, 0));

    let mut max_distance = 0;
    while let Some((point, d)) = q.pop_front()
    {
        if let Some(row) = diagram.get(point.x)
        {
            if let Some(col) = row.get(point.y)
            {
                if distances[point.x][point.y] == -1
                {
                    distances[point.x][point.y] = d;
                    max_distance = std::cmp::max(max_distance, d);

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
    max_distance
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

fn main()
{
    let lines = aoc_core::input_to_lines();

    let mut diagram = Vec::new();
    let mut distances = Vec::new();
    for line in lines.iter()
    {
        let mut row = Vec::new();
        let mut row_d = Vec::new();
        for c in line.chars()
        {
            row.push(c);
            row_d.push(-1);
        }
        diagram.push(row);
        distances.push(row_d);
    }

    let s = find_start(&diagram);
    // Determine the starting pipe shape so its easier to just run through the traverse algorithm.
    determine_s_pipe(&s, &mut diagram);

    let max_distance = traverse(s, &diagram, &mut distances);
    println!("{}", max_distance);
}
