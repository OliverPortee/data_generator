
#include <iostream>

#include "do_it.hpp"
#include "model.hpp"

Settings parse_cli_parameters(int argc, char* argv[]) {
    // --min NUMBER
    // --max NUMBER
    // -n COUNT # default 1000
    // -r COUNT # default 5
    // --seed NUMBER
    // -o csv|sql # default csv
    // --distribution uniform|normal # default uniform

    unsigned int pos = 1;
    while (pos < argc) {
        if (argv[pos])
    }
}

int main(int argc, char* argv[]) {
    Settings s{0,
               1000000,
               5,
               10,
               std::make_unique<CsvOutput>(),
               std::make_unique<UniformDistribution>(0, 1000000)};
    generate_data(s);
    return 0;
}
