use aoc_core;

enum Color
{
    Red,
    Green,
    Blue,
}

struct Game
{
    id: i64,
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

        games.push(Game { id, reveals });
    }

    let mut sum: i64 = 0;
    for game in games.iter()
    {
        let mut valid = true;
        for (color, amount) in game.reveals.iter()
        {
            valid = match color
            {
                Color::Red => *amount <= 12,
                Color::Green => *amount <= 13,
                Color::Blue => *amount <= 14,
            };

            if !valid
            {
                break;
            }
        }

        if valid
        {
            sum += game.id;
        }
    }
    println!("{}", sum);
}
