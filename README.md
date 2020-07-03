# adventofcode

https://adventofcode.com

This project uses git submodules for some libraries

    git clone --recurse-submodules https://github.com/jbaldwin/adventofcode.git

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
