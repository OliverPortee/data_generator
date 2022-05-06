
#include <iostream>

#include "do_it.hpp"
#include "model.hpp"

int main(int argc, char* argv[]) {
    Settings s{0,
               1000000,
               5,
               10,
               std::make_unique<CsvOutput>(),
               std::make_unique<UniformDistribution>(0, 1000000)};
    do_it(s);
    return 0;
}
