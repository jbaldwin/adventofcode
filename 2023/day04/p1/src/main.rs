use aoc_core;

use std::collections::HashSet;

fn main() {
    let lines = aoc_core::input_to_lines();

    let mut points = 0;
    for line in lines.iter()
    {
        let card_parts = aoc_core::str_split_str(line, ": ");
        let number_parts = aoc_core::str_split_str(card_parts[1].as_str(), " | ");

        let winning_numbers_parts = aoc_core::str_split_char(number_parts[0].as_str(), ' ');
        let mut winning_numbers = HashSet::new();
        for wnp in winning_numbers_parts.iter()
        {
            if wnp.len() > 0
            {
                winning_numbers.insert(wnp.parse::<i64>().unwrap());
            }
        }

        let our_numbers_parts = aoc_core::str_split_char(number_parts[1].as_str(), ' ');
        let mut our_numbers = Vec::new();
        for onp in our_numbers_parts.iter()
        {
            if onp.len() > 0
            {
                our_numbers.push(onp.parse::<i64>().unwrap());
            }
        }

        let mut card_points: i64 = 0;
        for n in our_numbers.iter()
        {
            if winning_numbers.contains(n)
            {
                if card_points == 0
                {
                    card_points += 1;
                }
                else
                {
                    card_points *= 2;
                }
            }
        }
        points += card_points;
    }

    println!("{}", points);

}
