use aoc_core;

enum Color
{
    Red,
    Green,
    Blue,
}

struct Game
{
    _id: i64,
    reveals: Vec<(Color, i64)>,
}

fn main() {
    let lines = aoc_core::input_to_lines();

    let mut games = Vec::new();

    for line in lines.iter()
    {
        let game_str = aoc_core::str_split_str(line, ": ");
        let id = game_str[0][5..].parse::<i64>().unwrap();
        let reveals_strs = aoc_core::str_split_str(&game_str[1], "; ");

        let mut reveals = Vec::new();
        for reveal_str in reveals_strs.iter()
        {
            let cubes_strs = aoc_core::str_split_str(reveal_str, ", ");
            for cube_str in cubes_strs.iter()
            {
                let cube_parts = aoc_core::str_split_char(cube_str, ' ');
                let amount = cube_parts[0].parse::<i64>().unwrap();
                let color = match cube_parts[1].as_str()
                {
                    "red" => Color::Red,
                    "green" => Color::Green,
                    "blue" => Color::Blue,
                    _ => panic!("unknown color"),
                };

                reveals.push((color, amount));
            }
        }

        games.push(Game { _id: id, reveals });
    }

    let mut sum: i64 = 0;
    for game in games.iter()
    {
        let mut min_red = 0;
        let mut min_green = 0;
        let mut min_blue = 0;
        for (color, amount) in game.reveals.iter()
        {
            match color
            {
                Color::Red => min_red = std::cmp::max(min_red, *amount),
                Color::Green => min_green = std::cmp::max(min_green, *amount),
                Color::Blue => min_blue = std::cmp::max(min_blue, *amount),
            };
        }

        let power = min_red * min_green * min_blue;
        sum += power;
    }
    println!("{}", sum);
}
