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
    let mut signal_strengths = Vec::new();

    for (n, i) in instructions.iter().enumerate() {
        match n + 1 {
            20 => {
                signal_strengths.push(x * 20);
            },
            60 => {
                signal_strengths.push(x * 60);
            },
            100 => {
                signal_strengths.push(x * 100);
            },
            140 => {
                signal_strengths.push(x * 140);
            },
            180 => {
                signal_strengths.push(x * 180);
            },
            220 => {
                signal_strengths.push(x * 220);
            },
            _ => { }
        };

        match i {
            Instruction::Noop => { },
            Instruction::Addx{amount} => { x += amount; }
        };
    }

    let sum: i64 = signal_strengths.iter().sum();
    println!("{}", sum);
}
