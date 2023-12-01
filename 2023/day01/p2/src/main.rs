use aoc_core;

const NUMBERS_AS_STRING: [(&'static str, char); 18] = 
[
    ("one", '1'),
    ("two", '2'),
    ("three", '3'),
    ("four", '4'),
    ("five", '5'),
    ("six", '6'),
    ("seven", '7'),
    ("eight", '8'),
    ("nine", '9'),
    ("eno", '1'),
    ("owt", '2'),
    ("eerht", '3'),
    ("ruof", '4'),
    ("evif", '5'),
    ("xis", '6'),
    ("neves", '7'),
    ("thgie", '8'),
    ("enin", '9'),
];

fn find_digit(input: &str) -> char
{
    let mut s = input;
    while s.len() > 0
    {
        let first = s.chars().nth(0).unwrap();
        if first.is_digit(10)
        {
            return first;
        }

        for (n, c) in NUMBERS_AS_STRING
        {
            if s.starts_with(n)
            {
                return c;
            } 
        }

        s = &s[1..];
    }

    panic!("unable to find a digit");
}

fn main() {
    let lines = aoc_core::input_to_lines();

    let mut sum = 0i64;
    for line in lines.iter()
    {
        let number: i64 = vec![
            find_digit(line),
            find_digit(line.chars().rev().collect::<String>().as_str()),
        ].into_iter().collect::<String>().parse::<i64>().unwrap();

        sum += number;
    }

    println!("{sum}");
}
