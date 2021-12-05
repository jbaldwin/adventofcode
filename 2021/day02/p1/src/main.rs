fn main() {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < 2 {
        println!("{} <input_file>", args[0]);
        std::process::exit(-1);
    }

    let cmds: Vec<String> = std::fs::read_to_string(&args[1])
        .unwrap()
        .split('\n')
        .into_iter()
        .map(|s| s.to_string())
        .collect();

    let mut pos_h: i64 = 0;
    let mut pos_d: i64 = 0;

    for cmd in cmds {
        let parts: Vec<&str> = cmd.split(' ').collect();
        match (parts[0], parts[1]) {
            ("forward", x) => {
                pos_h += x.parse::<i64>().unwrap();
            }
            ("down", x) => {
                pos_d += x.parse::<i64>().unwrap();
            }
            ("up", x) => {
                pos_d -= x.parse::<i64>().unwrap();
            }
            _ => panic!("unknown cmd {}", cmd),
        }
    }

    println!(
        "horizontal={}, depth={}, product={}",
        pos_h,
        pos_d,
        pos_h * pos_d
    );
}
