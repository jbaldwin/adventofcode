use aoc_core;

enum Instruction {
    Noop,
    Addx{amount: i64},
}

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');

    let mut instructions = Vec::new();

    for line in lines.iter() {
        match &line[0..4] {
            "noop" => {
                instructions.push(Instruction::Noop);
            },
            "addx" => {
                let amount = line[5..].parse::<i64>().unwrap();

                instructions.push(Instruction::Noop);
                instructions.push(Instruction::Addx{ amount });
            },
            _ => panic!("invalid input {}", line),
        }
    }

    let mut x: i64 = 1;
    let mut screen = Vec::new();

    for (n, i) in instructions.iter().enumerate() {
        let col: i64 = (n % 40).try_into().unwrap();
        if col == 0 {
            screen.push(Vec::new());
        }

        let row = screen.last_mut().unwrap();

        let diff = x - col;
        if diff >= -1 && diff <= 1 {
            row.push('#');
        }
        else {
            row.push(' ');
        }

        match i {
            Instruction::Noop => { },
            Instruction::Addx{amount} => { x += amount; }
        };
    }

    for row in screen.iter() {
        for col in row.iter() {
            print!("{}", col);
        }
        println!();
    }
}
