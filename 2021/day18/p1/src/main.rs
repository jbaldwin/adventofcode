use aoc_core;

enum Element {
    Value(u64),
    Pair(SnailFish),
}

struct SnailFish {
    x: Box<Element>,
    y: Box<Element>,
}

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');

    let mut snail_fish = Vec::new();
    for line in lines.iter() {
        snail_fish.push(parse_snailfish(line));
    }

}

fn impl_parse_snailfish(data: &str) -> Element {

}

fn parse_snailfish(data: &str) -> SnailFish {
    let c_iter = data.chars();

    match c_iter.nth(0) {
        '[' => {

        },
        ']' => {

        },
        ',' => {
            
        }
    }


    
    let x = match c_iter.nth(1).unwrap() {
        '[' => {
            Element::Pair(parse_snailfish(&data[2..]))
        },
        _ => {
            let d = &data[1..];
            Element::Value(d.parse::<u64>().unwrap())
        }
    };


}
