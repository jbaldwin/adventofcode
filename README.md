# adventofcode

https://adventofcode.com

## Cloning this repository
This project uses git submodules for some libraries on the C++ solutions, the Rust solutions use Cargo to properly build dependencies.

    git clone --recurse-submodules https://github.com/jbaldwin/adventofcode.git

## Rust Solutions
Moving forward I am solving the Advent of Code problems using rust, I have not introduced a root makefile or build system yet to build all Rust and C++ solutions, so depending on the implementation you'll need to run both make systems to build everything.  For Rust solutions simply use cargo to build and run the executables:

    # Build the rust problem solutions
    cargo build --release

    # Run a specific problem with the following template:
    cargo run --release --bin <cargo project name> -- <year>/<day>/<problem>/src/input.txt

    # Example:
    cargo run --release --bin aoc_2021_day01_p1 -- 2021/day01/p1/src/input.txt

## C++ Solutions
I originally started solving Advent of Code to hone my C++ skills, to build these problems use the following commands to build and run the problem solutions:

To build every solution availabile:

    mkdir Release && cd Release
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make

To run a solution from ./Release directory:

    # This will output the required arguments, generally just the input.txt is needed.
    ./bin/<year>_day<day>_p<part>

    # The input files live in the source directory for the
    # year/day/part, and most p2 parts just use p1 input.txt
    ./bin/2019_day18_p2 ../2019/day18/p1/input.txt
