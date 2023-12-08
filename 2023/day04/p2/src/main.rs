use aoc_core;

use std::collections::HashSet;

struct Card
{
    winning_numbers: HashSet<i64>,
    numbers: Vec<i64>,
    copies: usize,
}

impl Card
{
    fn new(winning_numbers: HashSet<i64>, numbers: Vec<i64>) -> Card
    {
        Card
        {
            winning_numbers,
            numbers,
            copies: 1,
        }
    }

    fn matches(&self) -> (usize, usize)
    {
        let mut sum = 0;
        for n in self.numbers.iter()
        {
            if self.winning_numbers.contains(n)
            {
                sum += 1;
            }
        }
        (sum, self.copies)
    }
}

fn main() {
    let lines = aoc_core::input_to_lines();

    let mut cards = Vec::new();
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

        let numbers_parts = aoc_core::str_split_char(number_parts[1].as_str(), ' ');
        let mut numbers = Vec::new();
        for onp in numbers_parts.iter()
        {
            if onp.len() > 0
            {
                numbers.push(onp.parse::<i64>().unwrap());
            }
        }

        cards.push(Card::new(winning_numbers, numbers));
    }

    for i in 0..cards.len()
    {
        let card = &cards[i];
        let (depth, amount) = card.matches();

        // Increment copies for all the following cards.
        for j in (i + 1)..(i + 1 + depth)
        {
            if j < cards.len()
            {
                cards[j].copies += amount;
            }
        }
    }

    let mut scratchcards = 0;
    for card in cards.iter()
    {
        scratchcards += card.copies;
    }
    println!("{}", scratchcards);

}
