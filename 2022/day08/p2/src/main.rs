use aoc_core;

fn scenic_score(trees: &Vec<Vec<u32>>, x: usize, y: usize) -> usize {
    // Anything on the edge has a viewing distance of zero, making it all zero.
    if x == 0 || y == 0 || x == trees.len() - 1 || y == trees[0].len() - 1 {
        return 0;
    }

    let my_height = trees[x][y];

    let mut up: usize = 0;
    for nx in (0..x).rev() {
        up += 1;
        if trees[nx][y] >= my_height {
            break;
        }
    }

    let mut down: usize = 0;
    for nx in (x + 1)..trees.len() {
        down += 1;
        if trees[nx][y] >= my_height {
            break;
        }
    }

    let mut left: usize = 0;
    for ny in (0..y).rev() {
        left += 1;
        if trees[x][ny] >= my_height {
            break;
        }
    }

    let mut right: usize = 0;
    for ny in (y + 1)..trees[0].len() {
        right += 1;
        if trees[x][ny] >= my_height {
            break;
        }
    }
    
    return up * down * left * right;
}

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');

    let mut trees = Vec::new();
    for line in lines.iter() {
        trees.push(Vec::new());
        let row = trees.last_mut().unwrap();

        for n in line.chars() {
            row.push(n.to_digit(10).unwrap());
        }
    }

    let mut max_scenic_score: usize = 0;
    for (x, row) in trees.iter().enumerate() {
        for (y, _col) in row.iter().enumerate() {
            let score = scenic_score(&trees, x, y);
            max_scenic_score = std::cmp::max(score, max_scenic_score);
        }
    }

    println!("{}", max_scenic_score);
}
