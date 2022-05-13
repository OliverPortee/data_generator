
#include <iostream>

#include "data_generation.hpp"
#include "model.hpp"

#if 0
Settings parse_cli_parameters(int argc, char* argv[]) {
    // --min NUMBER
    // --max NUMBER
    // -n COUNT # default 1000
    // -r COUNT # default 5
    // --seed NUMBER
    // -o csv|sql # default csv
    // --distribution uniform|normal # default uniform

    unsigned int pos = 1;
}
#endif

int main(int argc, char* argv[]) {
    Settings s{0,
               1000000,
               5,
               10,
               std::make_unique<CsvOutput>(),
               std::make_unique<UniformDistribution>(0, 1000000)};
    auto data = generate_data(s);
    (*s.output)(data, s, std::cout);
    return 0;
}
