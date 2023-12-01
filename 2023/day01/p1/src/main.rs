use aoc_core;


fn find_digit<I>(input: I) -> char
where
    I: Iterator<Item = char>,
{
    for c in input
    {
        if c.is_digit(10)
        {
            return c;
        }
    }

    panic!("unable to find a digit");
}

fn main() {
    let lines = aoc_core::input_to_lines();

    let mut sum = 0i64;
    for line in lines.iter()
    {
        let number: i64 = vec![
            find_digit(line.chars()),
            find_digit(line.chars().rev()),
        ].into_iter().collect::<String>().parse::<i64>().unwrap();

        sum += number;
    }

    println!("{sum}");
}
