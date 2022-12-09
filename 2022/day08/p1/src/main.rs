use aoc_core;

fn print_trees(trees: &Vec<Vec<u32>>) {
    for row in trees.iter() {
        for col in row.iter() {
            print!("{}", col);
        }
        println!();
    }
}

fn is_visible(trees: &Vec<Vec<u32>>, x: usize, y: usize) -> bool {
    // Anything on the edge is always visible.
    if x == 0 || y == 0 || x == trees.len() - 1 || y == trees[0].len() - 1 {
        return true;
    }

    let my_height = trees[x][y];

    let mut up =  true;
    for nx in 0..x {
        if trees[nx][y] >= my_height {
            up = false;
            break;
        }
    }
    if up {
        return true;
    }

    let mut down = true;
    for nx in (x + 1)..trees.len() {
        if trees[nx][y] >= my_height {
            down = false;
            break;
        }
    }
    if down {
        return true;
    }

    let mut left = true;
    for ny in 0..y {
        if trees[x][ny] >= my_height {
            left = false;
            break;
        }
    }
    if left {
        return true;
    }

    let mut right = true;
    for ny in (y + 1)..trees[0].len() {
        if trees[x][ny] >= my_height {
            right = false;
            break;
        }
    }
    if right {
        return true;
    }

    false
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

    let mut visible_trees: usize = 0;
    for (x, row) in trees.iter().enumerate() {
        for (y, _col) in row.iter().enumerate() {
            if is_visible(&trees, x, y) {
                visible_trees += 1;
            }
        }
    }

    println!("{}", visible_trees);
}
