pub fn args_init(required_args: usize) -> Vec<String> {
    let args: Vec<String> = std::env::args().collect();
    if args.len() < required_args {
        println!("{} <input_file>", args[0]);
        std::process::exit(-1);
    }
    args
}

pub fn file_read_delim(file: &str, delim: char) -> Vec<String> {
    let contents = std::fs::read_to_string(file).unwrap();
    str_split_char(&contents, delim)
}

pub fn str_split_char(s: &str, delim: char) -> Vec<String> {
    let parts: Vec<String> = s.split(delim).into_iter().map(|s| s.to_string()).collect();
    parts
}

pub fn str_split_str(s: &str, delim: &str) -> Vec<String> {
    let parts: Vec<String> = s.split(delim).into_iter().map(|s| s.to_string()).collect();
    parts
}
