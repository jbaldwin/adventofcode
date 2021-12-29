use aoc_core;
use std::collections::HashMap;

type PairInsertionRules = HashMap<(char, char), char>;
type Polymers = HashMap<(char, char), u64>;

fn main() {
    let args = aoc_core::args_init(3);
    let steps = args[2].parse::<usize>().unwrap();
    let lines = aoc_core::file_read_delim(&args[1], '\n');
    let mut lines_iter = lines.iter();

    let template = lines_iter.next().unwrap().clone();
    lines_iter.next(); // skip blank line

    let mut rules: PairInsertionRules = HashMap::new();

    for line in lines_iter {
        let parts = aoc_core::str_split_str(line, " -> ");
        let mut iter = parts[0].chars();
        rules.insert(
            (iter.next().unwrap(), iter.next().unwrap()),
            parts[1].chars().next().unwrap(),
        );
    }

    // Keep track of the polymers available for applying the rules to and their counts.
    let mut polymers: Polymers = HashMap::new();
    // Keep track of individual letter counts since we don't track the position of
    // the two letter polymers in this solution, that would result is massive over counting.
    let mut counts: HashMap<char, u64> = HashMap::new();

    // Pre-seed counts with given template.
    let mut char_iter = template.chars().peekable();
    loop {
        if let Some(key_a) = char_iter.next() {
            if let Some(key_b) = char_iter.peek() {
                let key = (key_a, *key_b);
                *polymers.entry(key).or_insert(0) += 1;
                *counts.entry(key_a).or_insert(0) += 1;
            } else {
                // The last item is always +1... and will _never_ change since you
                // cannot insert after it.
                *counts.entry(key_a).or_insert(0) += 1;
            }
        } else {
            break;
        }
    }

    for _step in 1..=steps {
        let mut next: Polymers = polymers.clone();

        for (k, v) in polymers.iter() {
            if let Some(c) = rules.get(&k) {
                // Remove self from polymers, this is guaranteed to exist.
                *next.get_mut(k).unwrap() -= *v;

                // Split into two entries and insert.
                let k1 = (k.0, *c);
                let k2 = (*c, k.1);

                *next.entry(k1).or_insert(0) += *v;
                *next.entry(k2).or_insert(0) += *v;

                // Increase increment on injected polymer.
                *counts.entry(*c).or_insert(0) += v;
            } else {
                *next.entry(*k).or_insert(0) += *v;
            }
        }

        polymers = next;
    }

    let mut min: u64 = u64::MAX;
    let mut max: u64 = 0;

    for (k, v) in counts {
        println!("{} = {}", k, v);

        min = std::cmp::min(min, v);
        max = std::cmp::max(max, v);
    }

    println!("{}", (max - min));
}
