use std::collections::HashMap;
use std::cmp::Ordering;

use aoc_core;

#[derive(PartialEq, PartialOrd)]
enum Type
{
    FiveOfAKind = 1,
    FourOfAKind = 2,
    FullHouse = 3,
    ThreeOfAKind = 4,
    TwoPair = 5,
    OnePair = 6,
    HighCard = 7,
}

static REPLACE_CARDS: &'static [char] = &['2', '3', '4', '5', '6', '7', '8', '9', 'T', 'Q', 'K', 'A'];

struct Hand
{
    cards: [char; 5],
    bid: i64,
    hand_type: Type,
}

impl Hand
{
    fn new(cards: [char; 5], bid: i64) -> Hand
    {
        let hand_type = Hand::hand_type(&cards);
        Hand
        {
            cards,
            bid,
            hand_type,
        }
    }

    fn hand_type(cards: &[char; 5]) -> Type
    {
        let mut counts = HashMap::new();
        for c in cards.iter()
        {
            *counts.entry(c).or_insert(0) += 1;
        }

        if counts.contains_key(&'J')
        {
            let mut best_hand_type = Type::HighCard;
            for i in 0..5
            {
                if cards[i] == 'J'
                {
                    for c in REPLACE_CARDS
                    {
                        let mut c2 = cards.clone();
                        c2[i] = *c;
                        let t = Hand::hand_type(&c2);

                        if t < best_hand_type
                        {
                            best_hand_type = t;
                        }
                    }
                }
            }

            return best_hand_type;
        }
        else
        {
            if counts.len() == 1
            {
                return Type::FiveOfAKind;
            }
            if counts.len() == 2
            {
                for (_, n) in counts.iter()
                {
                    if *n == 4 || *n == 1
                    {
                        return Type::FourOfAKind;
                    }
                }
                return Type::FullHouse;
            }
            if counts.len() == 3
            {
                for (_, n) in counts.iter()
                {
                    if *n == 3
                    {
                        return Type::ThreeOfAKind;
                    }
                }
                return Type::TwoPair;
            }
            if counts.len() == 4
            {
                return Type::OnePair;
            }

            return Type::HighCard;
        }

    }
}

fn card_value(c: char) -> i64
{
    match c
    {
        'J' => 1,
        '2' => 2,
        '3' => 3,
        '4' => 4,
        '5' => 5,
        '6' => 6,
        '7' => 7,
        '8' => 8,
        '9' => 9,
        'T' => 10,
        'Q' => 12,
        'K' => 13,
        'A' => 14,
        _ => panic!("unknown card type"),
    }
}

fn main()
{
    let lines = aoc_core::input_to_lines();

    let mut hands = Vec::new();
    for line in lines.iter()
    {
        let parts = aoc_core::str_split_char(line, ' ');
        hands.push(Hand::new(
            parts[0].chars().collect::<Vec<_>>().try_into().unwrap(),
            parts[1].parse::<i64>().unwrap(),
        ));
    }

    hands.sort_by(|a, b| {
        if a.hand_type < b.hand_type
        {
            return Ordering::Greater;
        }
        if a.hand_type > b.hand_type
        {
           return  Ordering::Less;
        }

        for i in 0..5
        {
            let av = card_value(a.cards[i]);
            let bv = card_value(b.cards[i]);

            if av > bv
            {
                return Ordering::Greater;
            }
            if av < bv
            {
                return Ordering::Less;
            }
        }

        return Ordering::Equal;
    });

    let mut total_winnings = 0;
    for (i, hand) in hands.iter().enumerate()
    {
        let bid_rank = (i + 1) * hand.bid as usize;
        total_winnings += bid_rank;
    }
    println!("{}", total_winnings);
}
