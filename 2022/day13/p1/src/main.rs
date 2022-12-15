use core::panic;

use aoc_core;

#[derive(Clone)]
enum Value {
    Integer(i64),
    List(Vec<Value>),
}

impl std::fmt::Display for Value {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        match self {
            Value::Integer(i) => {
                write!(f, "{}", i)
            },
            Value::List(l) => {
                let r = write!(f, "[");
                if r.is_err() {
                    return r;
                }
                let mut first = true;
                for value in l.iter() {
                    if !first {
                        let r = write!(f, ",{}", value);
                        if r.is_err() {
                            return r;
                        }
                    }
                    else {
                        first = false;
                        let r = write!(f, "{}", value);
                        if r.is_err() {
                            return r;
                        }
                    }
                }
                write!(f, "]")
            },
        }
    }
}

fn parse_integer(s: &str) -> (Value, usize) {
    let mut stop_idx: usize = 0;
    for c in s.chars() {
        match c {
            '0'..='9' => stop_idx += 1,
            _ => break,
        };
    }

    let i_str = &s[..stop_idx];
    (Value::Integer(i_str.parse::<i64>().unwrap()), i_str.len())
}

fn parse_list(s: &str) -> (Value, usize) {
    let mut values = Vec::new();
    let mut i: usize = 1; // strip first '['

    while i < s.len() - 1 { // strip final ']'
        let c = s.chars().nth(i).unwrap();
        match c {
            '[' => {
                let (l, amount) = parse_list(&s[i..]);
                values.push(l);
                i += amount;
            },
            ']' => {
                return (Value::List(values), i + 1);
            },
            ',' => {
                // continue
                i += 1;
            },
            _ => {
                let (v, amount) = parse_integer(&s[i..]);
                values.push(v);
                i += amount;
            }
        }
    }

    (Value::List(values), i)
}

fn parse_packet(s: &str) -> Value {
    let (v, _) = parse_list(s);
    v
}

enum Result {
    RightOrder,
    WrongOrder,
    Continue,
}

fn compare_list(l1: &Vec<Value>, l2: &Vec<Value>) -> Result {

    for (i, v1) in l1.iter().enumerate() {
        // If we've exhausted all comparisons and the left side is bigger 
        // then its in the wrong order, left is bigger.
        if i >= l2.len() {
            return Result::WrongOrder;
        }

        let v2 = l2.get(i).unwrap();

        // If both values are integers do comparison rules
        if  let (Value::Integer(i1), Value::Integer(i2)) = (v1, v2) {
            
            if i1 < i2 {
                return Result::RightOrder;
            }
            if i1 > i2 {
                return Result::WrongOrder;
            }
        }
        // if they are both lists, recurse
        else if let (Value::List(l1), Value::List(l2)) = (v1, v2) {
            match compare_list(l1, l2) {
                Result::RightOrder => return Result::RightOrder,
                Result::WrongOrder => return Result::WrongOrder,
                Result::Continue => continue,
            };
        }
        // If mismatched, promote the integer to a list
        else if let (Value::Integer(_i), Value::List(l2)) = (v1, v2) {
            let l1 = vec![v1.clone()];
            match compare_list(&l1, l2) {
                Result::RightOrder => return Result::RightOrder,
                Result::WrongOrder => return Result::WrongOrder,
                Result::Continue => continue,
            };

        }
        else if let (Value::List(l1), Value::Integer(_i)) = (v1, v2) {
            let l2 = vec![v2.clone()];
            match compare_list(l1, &l2) {
                Result::RightOrder => return Result::RightOrder,
                Result::WrongOrder => return Result::WrongOrder,
                Result::Continue => continue,
            };
        }
        else {
            panic!("types don't work.")
        }
    }

    // If there are more items in the right item its in the right order.
    if l2.len() > l1.len() {
        return Result::RightOrder;
    }
    // If the lists were identical length with identical comparisons continue.
    Result::Continue
}

fn compare_packets(p1: &Value, p2: &Value) -> bool {
    match p1 {
        Value::List(l1) => {
            match p2 {
                Value::List(l2) => {
                    match compare_list(l1, l2) {
                        Result::RightOrder => true,
                        Result::WrongOrder => false,
                        Result::Continue => false,
                    }
                },
                _ => panic!("invalid packet input2")
            }
        }
        _ => panic!("invalid packet input1")
    }
}

fn main() {
    let args = aoc_core::args_init(2);
    let contents = std::fs::read_to_string(&args[1]).unwrap();
    let lines = aoc_core::str_split_char(&contents, '\n');

    let mut pairs = Vec::new();

    for set in lines.chunks(3) {
        let p1 = parse_packet(set.get(0).unwrap());
        let p2 = parse_packet(set.get(1).unwrap());
        pairs.push((p1, p2));
    }

    let mut indicies = Vec::new();
    for (i, pair) in pairs.iter_mut().enumerate() {
        if compare_packets(&mut pair.0, &mut pair.1) {
            indicies.push(i + 1);
        }
    }

    println!("{:?}", indicies);
    println!("{}", indicies.iter().sum::<usize>());

}
