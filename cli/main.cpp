
#include "data_generation.hpp"

#include <cxxopts.hpp>
#include <iostream>
#include <optional>
#include <limits>
#include <cstring>
#include <string>
#include <sstream>


struct CliOptions {
    int min = std::numeric_limits<int>::min(); // --min
    int max = std::numeric_limits<int>::max(); // --max
    unsigned int sample_count = 1000;          // -n
    unsigned int col_count = 5;                // -c
    std::optional<unsigned int> seed;          // --seed
    std::string output = "csv";                // -o
    std::string distribution = "uniform";      // --distribution
};


template<class T>
T parse(const char* s) {
    std::istringstream iss(std::string{s});
    T t;
    iss >> t;
    return t;
}

Settings parse_cli_parameters(int argc, char* argv[]) {
    
    CliOptions options{};
    unsigned int i = 1;
    while (i < argc) {
        if (std::strcmp(argv[i], "--min") == 0) {
            if (++i >= argc) {
                // fail
            }
            options.min = parse<int>(argv[i]);
        } else if (std::strcmp(argv[i], "--max") == 0) {
            if (++i >= argc) {
                // fail
            }
            options.max = parse<int>(argv[i]);
        } else if (std::strcmp(argv[i], "-n") == 0) {
            if (++i >= argc) {
                // fail
            }
            options.sample_count = parse<unsigned int>(argv[i]);
        } else if (std::strcmp(argv[i], "-c") == 0) {
        }
    }
}

int main(int argc, char* argv[]) {
    auto settings = parse_cli_parameters(argc, argv);
    auto data = generate_data(settings);
    (*settings.output)(data, settings, std::cout);
    return 0;
}
