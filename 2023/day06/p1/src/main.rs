use aoc_core;

struct Race
{
    duration: i64,
    distance: i64,
}

fn main() {
    let acceleration = 1;

    let lines = aoc_core::input_to_lines();

    let mut races = Vec::new();
    let time_parts = aoc_core::str_split_char(lines[0].as_str(), ':');
    let times_parts = aoc_core::str_split_char(time_parts[1].as_str(), ' ');
    let distance_parts = aoc_core::str_split_char(lines[1].as_str(), ':');
    let distances_parts = aoc_core::str_split_char(distance_parts[1].as_str(), ' ');

    for times_part in times_parts.iter()
    {
        if times_part.len() > 0
        {
            races.push(Race {
                duration: times_part.parse::<i64>().unwrap(),
                distance: 0,
            });
        }
    }

    let mut i = 0;
    for distances_part in distances_parts.iter()
    {
        if distances_part.len() > 0
        {
            races[i].distance = distances_part.parse::<i64>().unwrap();
            i += 1;
        }
    }

    let mut product = 1; 
    for race in races.iter()
    {
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
        product *= sum;
    }

    println!("{}", product);

}
