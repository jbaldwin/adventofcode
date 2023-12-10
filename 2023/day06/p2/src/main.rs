use aoc_core;

struct Race
{
    duration: i64,
    distance: i64,
}

fn main() {
    let acceleration = 1;

    let lines = aoc_core::input_to_lines();

    let time_parts = aoc_core::str_split_char(lines[0].as_str(), ':');
    let duration = aoc_core::str_split_char(time_parts[1].as_str(), ' ').iter().fold(String::new(), |a, s| { if s.len() > 0 { a + s } else { a } }).parse::<i64>().unwrap();
    let distance_parts = aoc_core::str_split_char(lines[1].as_str(), ':');
    let distance = aoc_core::str_split_char(distance_parts[1].as_str(), ' ').iter().fold(String::new(), |a, s| { if s.len() > 0 { a + s } else { a } }).parse::<i64>().unwrap();

    let race = Race { duration, distance };

    let mut sum = 0;
    for t in 1..race.duration
    {
        let remaining = (race.duration - t).abs();
        let d = (t * acceleration) * remaining;
        if d > race.distance
        {
            sum += 1;
        }
    }

    println!("{}", sum);

}
