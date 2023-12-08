use aoc_core;

struct Seed
{
    id: usize,
    location: usize,
}

struct Map
{
    destination_range_start: usize,
    source_range_start: usize,
    range_length: usize,
}

impl Map
{
    fn contains(&self, pos: usize) -> bool
    {
        pos >= self.source_range_start && pos < self.source_range_start + self.range_length
    }

    fn map(&self, pos: usize) -> usize
    {
        if pos >= self.source_range_start && pos < self.source_range_start + self.range_length
        {
            if self.source_range_start > self.destination_range_start
            {
                return pos - (self.source_range_start - self.destination_range_start);
            }
            else
            {
                return pos + (self.destination_range_start - self.source_range_start);
            }
        }
        return pos;
    }
}

fn main() {
    let lines = aoc_core::input_to_lines();

    let mut seeds = Vec::new();
    let seeds_parts = aoc_core::str_split_str(lines[0].as_str(), ": ");
    let seeds_numbers: Vec<usize> = aoc_core::str_split_char(seeds_parts[1].as_str(), ' ').iter().map(|s| s.parse::<usize>().unwrap()).collect();
    for seed_number in seeds_numbers
    {
        seeds.push(Seed 
            { 
                id: seed_number,
                location: usize::MAX,
            }
        );
    }

    let mut maps = Vec::new();

    for line in lines.iter().skip(1)
    {
        if line.is_empty()
        {
            maps.push(Vec::new());
            continue;
        }

        if line.contains("map:")
        {
            // just skip this line, the maps are always ordered from one to the next.
            continue;
        }

        let parts: Vec<usize> = aoc_core::str_split_char(line.as_str(), ' ').iter().map(|s| s.parse::<usize>().unwrap()).collect();
        maps.last_mut().unwrap().push(Map {
            destination_range_start: parts[0],
            source_range_start: parts[1],
            range_length: parts[2],
        });
    }

    let mut lowest_location = usize::MAX;
    for seed in seeds.iter_mut()
    {
        let mut pos = seed.id;
        for maps in maps.iter()
        {
            for map in maps.iter()
            {
                if map.contains(pos)
                {
                    pos = map.map(pos);
                    break;
                }
            }
        }

        seed.location = pos;
        lowest_location = std::cmp::min(lowest_location, pos);
    }
    println!("{}", lowest_location);

}
