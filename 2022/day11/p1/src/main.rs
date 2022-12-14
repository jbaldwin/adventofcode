use aoc_core;
use std::collections::VecDeque;

enum Operand {
    Constant{value: usize},
    Old,
}

enum Operator {
    Addition,
    Multiply,
}

struct Operation {
    left: Operand,
    operator: Operator,
    right: Operand,
}

struct Monkey {
    id: usize,
    items: VecDeque<usize>,
    operation: Operation,
    test: usize,
    if_true: usize,
    if_false: usize,

    inspected: usize,
}

fn parse_items(s: &str) -> VecDeque<usize> {
    let mut items = VecDeque::new();
    {
        let parts = aoc_core::str_split_str(s, ": ");
        let nums = aoc_core::str_split_str(&parts[1], ", ");
        for num in nums.iter() {
            items.push_back(num.parse::<usize>().unwrap());
        }
    }
    items
}

fn parse_operand(s: &str) -> Operand {
    match s {
        "old" => Operand::Old,
        _ => Operand::Constant { value: s.parse::<usize>().unwrap() },
    }
}

fn parse_operator(s: &str) -> Operator {
    match s {
        "+" => Operator::Addition,
        "*" => Operator::Multiply,
        _ => panic!("invalid input {}", s),
    }
}

fn parse_operation(s: &str) -> Operation {
    let op_parts1 = aoc_core::str_split_str(s, "= ");
    let op_parts2 = aoc_core::str_split_char(&op_parts1[1], ' ');

    Operation {
        left: parse_operand(&op_parts2[0]),
        operator: parse_operator(&op_parts2[1]),
        right: parse_operand(&op_parts2[2]),
    }
}

fn interpret(item: usize, op: &Operation) -> usize {
    let left = match op.left {
        Operand::Old => item,
        Operand::Constant { value } => value,
    };

    let right = match op.right {
        Operand::Old => item,
        Operand::Constant { value } => value,
    };

    match op.operator {
        Operator::Addition => left + right,
        Operator::Multiply => left * right,
    }
}

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');

    let mut monkeys = Vec::new();

    let chunks = lines.chunks(7);
    for chunk in chunks {
        monkeys.push(Monkey {
            id: chunk[0].chars().nth(7).unwrap().to_digit(10).unwrap() as usize,
            items: parse_items(&chunk[1]),
            operation: parse_operation(&chunk[2]),
            test: chunk[3][21..].parse::<usize>().unwrap(),
            if_true: chunk[4][29..].parse::<usize>().unwrap(),
            if_false: chunk[5][30..].parse::<usize>().unwrap(),

            inspected: 0,
        });
    }

    for round in 1..=20 {
        for i in 0..monkeys.len() {
            let mut items = std::mem::replace(&mut monkeys.get_mut(i).unwrap().items, VecDeque::new());
            monkeys.get_mut(i).unwrap().inspected += items.len();
            while let Some(mut item) = items.pop_front() {
                item = interpret(item, &monkeys.get(i).unwrap().operation);
                item = item / 3;

                if item % monkeys.get(i).unwrap().test == 0 {
                    let to_idx = monkeys.get(i).unwrap().if_true;
                    monkeys.get_mut(to_idx).unwrap().items.push_back(item);
                } else {
                    let to_idx = monkeys.get(i).unwrap().if_false;
                    monkeys.get_mut(to_idx).unwrap().items.push_back(item);
                }
            }
        }

        println!("...\n");
        println!("After round {} the monkeys are holding items with these worry levels:", round);
        for monkey in monkeys.iter() {
            println!("Monkey {}: {:?}", monkey.id, monkey.items);
        }
    }

    let mut counts = Vec::new();

    for monkey in monkeys.iter() {
        println!("Monkey {} inspected items {} times.", monkey.id, monkey.inspected);
        counts.push(monkey.inspected);
    }

    counts.sort();
    counts.reverse();

    let monkey_business = counts.get(0).unwrap() * counts.get(1).unwrap();

    println!("{}", monkey_business);

}
