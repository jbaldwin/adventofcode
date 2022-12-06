use aoc_core;

struct Instruction {
    amount: u64,
    from: usize,
    to: usize,
}

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');

    let mut stacks = Vec::new();
    let mut parsing_instructions = false;
    let mut instructions = Vec::new();

    for line in lines.iter() {
        if line.len() == 0 {
            parsing_instructions = true;
            continue;
        }

        if !parsing_instructions {
            let mut stack_idx: usize = 0;
            let mut i: usize = 1;
            while i < line.len() {
                let c = line.chars().nth(i).unwrap();
                match c {
                    'A'..='Z' => {
                        while stack_idx >= stacks.len() {
                            stacks.push(Vec::new());
                        }

                        stacks[stack_idx].push(c);
                    },
                    _ => { }
                };

                stack_idx += 1;
                i += 4;
            }

        } else {
            // could probably continue searches from one to the next but this was easy enough
            let amount_idx = line.find("move ").unwrap() + 5;
            let from_idx = line.find("from ").unwrap() + 5;
            let to_idx = line.find("to ").unwrap() + 3;

            let amount_str = &line[amount_idx..(from_idx - 5)].trim();
            let from_str = &line[from_idx..(to_idx - 3)].trim();
            let to_str = &line[to_idx..].trim();

            instructions.push(Instruction {
                amount: amount_str.parse::<u64>().unwrap(),
                from: from_str.parse::<usize>().unwrap() - 1,
                to: to_str.parse::<usize>().unwrap() - 1,
            });
        }
    }

    // reverse so we can pop/push from the end of the vector and not move every item each time.
    for stack in stacks.iter_mut() {
        stack.reverse();
    }

    for instruction in instructions.iter() {
        let range = (stacks[instruction.from].len() - (instruction.amount as usize))..;

        let slice: &mut Vec<char> = &mut stacks[instruction.from].drain(range).collect::<Vec<char>>();
        slice.reverse(); // Mimic moving 1 at a time by reversing the order.

        stacks[instruction.to].append(slice);
        
    }

    for stack in stacks.iter() {
        if !stack.is_empty() {
            print!("{}", stack.last().unwrap());
        }
    }

}
