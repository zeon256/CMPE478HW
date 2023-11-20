# CMPE478 Homework 1 Fall 2023

## Team members
- Budi Syahiddin
- Ahmad Azfar

## What do we understand from the sample code?
The sample code provided is a simple way to find prime numbers 
by checking if a number can be prime by checking if it is possible to prime factorize it.
If a number can be prime factorized, then it is not a prime number.

## What did we do
- Refactor the sample code to use `for` loop for easier parallelization with OpenMP
- Use OpenMP to parallelize the code
- Use `std::chrono` to measure the time taken to run the code
- Use `std::ofstream` to write the results to a file

## Design decisions
- Modern C++ is used to make the code easier to read and understand
- `std::vector` is used to store the prime numbers
- `std::ofstream` is used to write the results to a file
- Use C++ templates to reduce code duplication
- Use C++20 concepts to make the code easier to read and understand

## How to run
### Using cmake (recommended)
Run the following command on the root directory of the project
```bash
cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Release -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=/opt/homebrew/opt/llvm/bin/clang -DCMAKE_CXX_COMPILER:FILEPATH=/opt/homebrew/opt/llvm/bin/clang++ -S<path_to_src> -B<path_to_src/build> -G Ninja
```

Then run the following command to build the project
```bash
cmake --build <path_to_src/build> --config Release --target all --
```

Make sure to replace `<path_to_src>` with the path to the source code.

Then run the following command to run the project
```bash
./build/cmpe478_hw1
```

### Using raw clang/gcc
Run the following command on the root directory of the project
```bash
clang++ -std=c++20 -fopenmp -O3 -o cmpe478_hw1 src/main.cpp
```

Then run the following command to run the project
```bash
./build/cmpe478_hw1
```


## Tested on
- MacOS Sonoma 14.1 (23B74)
- Apple Silicon M1
- 8GB RAM
- Apple Clang Version 17.0.3 (arm64-apple-darwin23.1.0)
- C++20
- OpenMP 17.0.5

## Results
- Output file is benchmark.csv
