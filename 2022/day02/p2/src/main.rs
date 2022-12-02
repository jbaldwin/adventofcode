use aoc_core;

enum Hands {
    Rock,
    Paper,
    Scissors,
}

enum Plays {
    Lose,
    Draw,
    Win,
}

const WIN: u64 = 6;
const DRAW: u64 = 3;
const LOSE: u64 = 0;

const ROCK: u64 = 1;
const PAPER: u64 = 2;
const SCISSORS: u64 = 3;

fn translate_hand(c: char) -> Hands {
    match c {
        'A' => Hands::Rock,
        'B' => Hands::Paper,
        'C' => Hands::Scissors,
        _ => panic!("invalid input {}", c),
    }
}

fn translate_play(c: char) -> Plays {
    match c {
        'X' => Plays::Lose,
        'Y' => Plays::Draw,
        'Z' => Plays::Win,
        _ => panic!("invalid input {}", c),
    }
}

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');

    let mut plays = Vec::new();
    for line in lines.iter() {
        plays.push((
            translate_hand(line.chars().nth(0).unwrap()),
            translate_play(line.chars().nth(2).unwrap())));
    }

    let mut score: u64 = 0;
    for (hand, play) in plays.iter() {
        score += match hand {
            Hands::Rock => {
                match play {
                    Plays::Win => WIN + PAPER,
                    Plays::Draw => DRAW + ROCK,
                    Plays::Lose => LOSE + SCISSORS,
                }
            },
            Hands::Paper => {
                match play {
                    Plays::Win => WIN + SCISSORS,
                    Plays::Draw => DRAW + PAPER,
                    Plays::Lose => LOSE + ROCK,
                }
            },
            Hands::Scissors => {
                match play {
                    Plays::Win => WIN + ROCK,
                    Plays::Draw => DRAW + SCISSORS,
                    Plays::Lose => LOSE + PAPER,
                }
            },
        }
    }

    println!("{}", score);
}
