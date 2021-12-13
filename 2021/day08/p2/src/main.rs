use itertools::Itertools;
use std::collections::HashMap;
use std::collections::HashSet;

type SlotType = (Vec<char>, String, i64);
type SlotsType = Vec<SlotType>;

fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 2 {
        println!("{} <input_file>", args[0]);
        std::process::exit(-1);
    }

    // Simple Example mapping:
    // cagedb  abcde g -> abcdeg  -> 0
    // ab      ab      -> ab      -> 1
    // gcdfa   a cd fg -> acdfg   -> 2
    // fbcad   abcd f  -> abcdf   -> 3
    // eafb    ab  ef  -> abef    -> 4
    // cdfbe    bcdef  -> bcdef   -> 5
    // cdfgeb   bcdefg -> bcdefg  -> 6
    // dab     ab d    -> abd     -> 7
    // acedgfb abcdefg -> abcdefg -> 8
    // cefabd  abcdef  -> abcdef  -> 9

    // Using line 1 from the input file:
    // cdeba gadec dafcgb bedg bd cdb cdebga fadecg adcfbeg bacfe | aedgbcf gcfead dbacfeg gecabd

    // dafcgb  abcd fg -> 0
    // bd       b d    -> 1
    // bacfe   abc ef  -> 2
    // cdeba   abcde   -> 3
    // bedg     b de g -> 4
    // gadec   a cde g -> 5
    // fadecg  a cdefg -> 6
    // cdb      bcd    -> 7
    // adcfbeg abcdefg -> 8
    // cdebga  abcde g -> 9

    //   c
    // g   b
    //   e
    // f   d
    //   a

    ///////////////////////////////////////

    // 0 1 2 3 4 5 6
    //   0
    // 1   2
    //   3
    // 4   5
    //   6

    let patterns: Vec<Vec<char>> = vec![
        //    0    1    2    3    4    5    6
        vec!['1', '1', '1', '0', '1', '1', '1'], // 0
        vec!['0', '0', '1', '0', '0', '1', '0'], // 1
        vec!['1', '0', '1', '1', '1', '0', '1'], // 2
        vec!['1', '0', '1', '1', '0', '1', '1'], // 3
        vec!['0', '1', '1', '1', '0', '1', '0'], // 4
        vec!['1', '1', '0', '1', '0', '1', '1'], // 5
        vec!['1', '1', '0', '1', '1', '1', '1'], // 6
        vec!['1', '0', '1', '0', '0', '1', '0'], // 7
        vec!['1', '1', '1', '1', '1', '1', '1'], // 8
        vec!['1', '1', '1', '1', '0', '1', '1'], // 9
    ];

    // Counts:
    // 2 => 1
    // 3 => 7
    // 4 => 4
    // 5 => 2, 3, 5
    // 6 => 0, 6, 9
    // 7 => 8

    let mut sum = 0;

    let lines: Vec<String> = std::fs::read_to_string(&args[1])
        .unwrap()
        .split('\n')
        .into_iter()
        .map(|s| s.to_string())
        .collect();

    for line in lines.iter() {
        let mut values: Vec<String> = Vec::new();
        let slots: SlotsType = Vec::new();

        let mut parts = line.split(" | ");
        let mut remaining: Vec<String> = parts
            .next()
            .unwrap()
            .split(' ')
            .into_iter()
            .map(|s| s.to_string())
            .collect();
        remaining.sort_by(|a, b| a.len().cmp(&b.len()));

        let values_parts = parts.next().unwrap().split(' ');
        for value in values_parts {
            values.push(value.chars().sorted().collect::<String>());
        }

        let fives_left: Vec<i64> = vec![2, 3, 5];
        let sixes_left: Vec<i64> = vec![0, 6, 9];

        let solution = solve(&patterns, slots, remaining, fives_left, sixes_left);

        let mut mapping: HashMap<String, char> = HashMap::new();
        if let Some(rows) = solution {
            for row in rows.iter() {
                mapping.insert(
                    row.1.clone().chars().sorted().collect::<String>(),
                    row.2.to_string().chars().next().unwrap(),
                );
            }
        }

        let mut i = String::new();
        for value in values.iter() {
            let v = mapping.get(value).unwrap();
            i.push_str(&v.to_string());
        }
        sum += i.parse::<i64>().unwrap();
    }

    println!("{}", sum);
}

fn column_check(slots: &SlotsType) -> bool {
    for i in 0..7 {
        let mut present: HashSet<char> = HashSet::new();

        for slot in slots.iter() {
            if slot.0[i] != '0' {
                present.insert(slot.0[i]);

                if present.len() > 1 {
                    return false;
                }
            }
        }
    }

    return true;
}

fn solve(
    patterns: &Vec<Vec<char>>,
    slots: SlotsType,
    remaining: Vec<String>,
    fives_left: Vec<i64>,
    sixes_left: Vec<i64>,
) -> Option<SlotsType> {
    if slots.len() == 10 {
        return Some(slots);
    }

    let key = &remaining[0];

    let mut possible_patterns: [i64; 3] = [-1, -1, -1];

    match key.len() {
        2 => {
            possible_patterns[0] = 1;
        }
        3 => {
            possible_patterns[0] = 7;
        }
        4 => {
            possible_patterns[0] = 4;
        }
        7 => {
            possible_patterns[0] = 8;
        }
        5 => {
            for i in 0..fives_left.len() {
                possible_patterns[i] = fives_left[i];
            }
        }
        6 => {
            for i in 0..sixes_left.len() {
                possible_patterns[i] = sixes_left[i];
            }
        }
        _ => {
            panic!("bad logic")
        }
    };

    let chars: Vec<char> = key.chars().collect();

    for possible_pattern_idx in possible_patterns {
        if possible_pattern_idx == -1 {
            return None;
        }

        for perm in chars.iter().permutations(chars.len()) {
            let mut slots_copy = slots.clone();
            let mut pattern = patterns[possible_pattern_idx as usize].clone();

            let mut count: usize = 0;

            for c in pattern.iter_mut() {
                if *c == '1' {
                    *c = *perm[count];
                    count += 1;
                }
            }

            slots_copy.push((pattern, key.clone(), possible_pattern_idx));

            if slots_copy.len() > 1 {
                if !column_check(&slots) {
                    return None;
                }
            }

            let mut remaining_copy = Vec::with_capacity(remaining.len());
            remaining_copy.extend_from_slice(&remaining[1..]);

            let mut fl = fives_left.clone();
            let mut sl = sixes_left.clone();

            fl.iter()
                .position(|&n| n == possible_pattern_idx)
                .map(|e| fl.remove(e));

            sl.iter()
                .position(|&n| n == possible_pattern_idx)
                .map(|e| sl.remove(e));

            if let Some(solution) = solve(&patterns, slots_copy, remaining_copy, fl, sl) {
                return Some(solution);
            }
        }
    }

    return None;
}
