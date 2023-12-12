use aoc_core;

fn main()
{
    let lines = aoc_core::input_to_lines();

    let mut reports = Vec::new();

    for line in lines.iter()
    {
        let values: Vec<i64> = aoc_core::str_split_char(line, ' ').iter().map(|s| s.parse::<i64>().unwrap()).collect();
        reports.push(values);
    }

    let mut sum = 0;
    for report in reports.iter()
    {
        let mut histories = Vec::new();
        histories.push(report.clone());

        loop
        {
            // Calculate diff and add new row.
            {
                let last_row = histories.last().unwrap();
                let mut iter = last_row.iter();
                let mut new_row = Vec::new();

                let mut last = *iter.next().unwrap();
                while let Some(n) = iter.next()
                {
                    new_row.push(*n - last);
                    last = *n;
                }

                histories.push(new_row);
            }

            // Check for end condition.
            {
                let last = histories.last().unwrap();
                let mut zeros = true;
                for n in last.iter()
                {
                    if *n != 0 
                    {
                        zeros = false;
                        break;
                    }
                }

                if zeros
                {
                    break;
                }
            }
        }

        // Add new item to the end of each history
        {
            let mut iter = histories.iter_mut().rev();
            *(iter.next().unwrap().last_mut().unwrap()) = 0;
            let mut last = 0;
            while let Some(h) = iter.next()
            {
                let l = *h.last().unwrap();
                h.push(last + l);
                last = last + l;
            }

            sum += histories[0].last().unwrap();
        }
    }
    println!("{}", sum);
}
