use aoc_core;

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();

    for i in 3..contents.len() {
        let slice = &contents[i-3..=i];

        let a = slice.chars().nth(0).unwrap();
        let b = slice.chars().nth(1).unwrap();
        let c = slice.chars().nth(2).unwrap();
        let d = slice.chars().nth(3).unwrap();

        if a != b && a != c && a != d 
            && b != c && b != d
            && c != d {
            
            println!("{} -> {}", (i+1), slice);
            break;
        }
    }
}
