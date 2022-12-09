use aoc_core;
use std::collections::HashMap;

struct Directory {
    name: String,
    children: HashMap<String, Directory>,
    files: HashMap<String, usize>,
}

impl Directory {
    fn new(name: String) -> Directory {
        Directory {
            name,
            children: HashMap::new(),
            files: HashMap::new(),
        }
    }

    fn size(&self, dirs: &mut Vec<(String, usize)>) -> usize {
        let mut s: usize = 0;

        for (_name, size) in self.files.iter() {
            s += size;
        }

        for (_name, dir) in self.children.iter() {
            s += dir.size(dirs);
        }

        if s <= 100000 {
            dirs.push((self.name.clone(), s));
        }

        s
    }

}

impl std::fmt::Display for Directory {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        let mut r = writeln!(f, "{}", self.name);
        if r.is_err() {
            return r;
        }

        for (name, dir) in self.children.iter() {
            r = writeln!(f, "dir {}", name);
            if r.is_err() {
                return r;
            }
            r = write!(f, "{}", dir);
            if r.is_err() {
                return r;
            }
        }

        for (name, size) in self.files.iter() {
            r = writeln!(f, "{} {}", name, size);
            if r.is_err() {
                return r;
            }
        }

        r
    }
}

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');

    let mut root = Directory::new("/".to_string());
    let mut cwd_path = Vec::new();

    let mut lines_iter = lines.iter().peekable();
    while let Some(line) = lines_iter.next() {
        if line.starts_with("$ cd") {
            let dir_name = &line[5..];

            if dir_name == ".." {
                // don't pop the root off
                if cwd_path.len() > 1 {
                    cwd_path.pop();
                }
            } else if dir_name == "/" {
                cwd_path.clear();
                cwd_path.push("/".to_string());
            } else {
                cwd_path.push(dir_name.to_string());
            }
        } else if line.starts_with("$ ls") {
            // travese down the tree and grab a reference to the cwd_path
            let mut cwd = &mut root;
            let mut iter = cwd_path.iter();
            iter.next();
            for d in iter {
                cwd = cwd.children.get_mut(d).unwrap();
            }

            // consume lines until we hit another command
            loop {
                {
                    match lines_iter.peek() {
                        Some(n) => {
                            if n.starts_with("$") {
                                break;
                            }
                        },
                        None => break,
                    }
                }
                let next = lines_iter.next().unwrap(); // consume

                if next.starts_with("dir") {
                    let dir_name = &next[4..];
                    cwd.children.insert(dir_name.to_string(), Directory::new(dir_name.to_string()));
                } else {
                    let parts = aoc_core::str_split_char(next, ' ');

                    let size = parts[0].parse::<usize>().unwrap();
                    let file_name = &parts[1];

                    cwd.files.insert(file_name.clone(), size);
                }
            }
        }
    }

    let mut small_dirs = Vec::new();
    root.size(&mut small_dirs);

    let total_size: usize = small_dirs.iter().map(|d| d.1).sum();
    println!("{}", total_size);
}
