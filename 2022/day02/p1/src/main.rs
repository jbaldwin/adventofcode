use aoc_core;

enum Hands {
    Rock,
    Paper,
    Scissors,
}

const WIN: u64 = 6;
const TIE: u64 = 3;
const LOSE: u64 = 0;

const ROCK: u64 = 1;
const PAPER: u64 = 2;
const SCISSORS: u64 = 3;

fn translate_play(c: char) -> Hands {
    match c {
        'A' => Hands::Rock,
        'B' => Hands::Paper,
        'C' => Hands::Scissors,
        'X' => Hands::Rock,
        'Y' => Hands::Paper,
        'Z' => Hands::Scissors,
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
            translate_play(line.chars().nth(0).unwrap()),
            translate_play(line.chars().nth(2).unwrap())));
    }

    let mut score: u64 = 0;
    for play in plays.iter() {
        score += match play.1 {
            Hands::Rock => ROCK,
            Hands::Paper => PAPER,
            Hands::Scissors => SCISSORS,
        };

        score += match play.1 {
            Hands::Rock => {
                match play.0 {
                    Hands::Rock => TIE,
                    Hands::Paper => LOSE,
                    Hands::Scissors => WIN,
                }
            },
            Hands::Paper => {
                match play.0 {
                    Hands::Rock => WIN,
                    Hands::Paper => TIE,
                    Hands::Scissors => LOSE,
                }
            },
            Hands::Scissors => {
                match play.0 {
                    Hands::Rock => LOSE,
                    Hands::Paper => WIN,
                    Hands::Scissors => TIE,
                }
            },
        }
    }

    println!("{}", score);
}
