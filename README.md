
# Data Generator

## Command line interface

```
Usage: ./gendata [OPTIONS] [SUBCOMMAND]

Options:
  -h,--help                   Print this help message and exit
  -n UINT:POSITIVE            number of rows to be generated
  -c UINT:POSITIVE            number of cols to be generated
  --seed UINT                 use seed for deterministic pseudo-random data
  -o,--output ENUM:value in {csv->0,json->2,sql->1} OR {0,2,1}
                              output format
  --tablename TEXT            tablename for sql output

Subcommands:
  uniform                     generates random integers from a uniform distribution
  normal                      generates random doubles from a normal distribution
  bernoulli                   generates random booleans from a bernoulli distribution
```

**examples**
* uniform distribution: `gendata -n 10 -c 4 --seed 0 --output sql --tablename my_table uniform --min -20 --max 400`
* normal distribution: `gendata -n 10 -c 4 --seed 0 --output csv normal --mean 5 --stddev 2`
* bernoulli distribution `gendata -n 10 -c 4 --seed 0 --output json bernoulli -p 0.8`
* when leaving out the random distribution subcommand, a uniform distribution is used
* writing to file (prints command to stderr): `gendata > output_file.csv`
* writing comment to file too: `gendata > output_file.csv 2>&1` (redirect stderr to stdout)

## Library

This can be used as a single header library. Just include `data_generator/data_generator.hpp`! Have a look at the [example](example.cpp).

## Build

* `mkdir build && cd build && cmake .. && make $TARGET`
* available targets:
	* `gendata` for command line interface
	* `example` for [example.cpp](example.cpp)
	* `tests` for the tests

## Dependencies

The dependencies will be fetched automatically by CMake.

* [Catch2](https://github.com/catchorg/Catch2), v3.0.1, for the tests
* [CLI11](https://github.com/CLIUtils/CLI11), v2.2.0, for the command line interface
