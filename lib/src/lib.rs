pub fn args_init(required_args: usize) -> Vec<String> {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < required_args {
        println!("{} <input_file>", args[0]);
        std::process::exit(-1);
    }
    args
}

pub fn file_read_delim(file: &str, delim: char) -> Vec<String> {
    let lines: Vec<String> = std::fs::read_to_string(file)
        .unwrap()
        .split(delim)
        .into_iter()
        .map(|s| s.to_string())
        .collect();
    lines
}
