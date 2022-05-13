
#include "data_generation.hpp"

#include "CLI/CLI.hpp"

#include <iostream>
#include <optional>
#include <limits>
#include <cstring>
#include <string>
#include <sstream>
#include <random>


struct CliOptions {
    int min = std::numeric_limits<int>::min();   // --min
    int max = std::numeric_limits<int>::max();   // --max
    unsigned int sample_count = 1000;            // -n
    unsigned int col_count = 5;                  // -c
    unsigned int seed = std::random_device{}();  // --seed
    std::string output = "csv";                  // -o
    std::string distribution = "uniform";        // --distribution
};

CliOptions parse_cli_options(int argc, char* argv[]) {
    CliOptions options{};
    CLI::App app{"random int generator"};
    auto min_option = app.add_option("--min", options.min, "minimum value of random ints");
    auto max_option = app.add_option("--max", options.max, "maximum value of random ints");
    app.add_option("-n", options.sample_count, "number of rows to be generated");
    app.add_option("-c", options.col_count, "number of cols to be generated");
    app.add_option("--seed", options.seed, "use seed for deterministic pseudo-random data");
    app.add_option("-o,--ouput", options.output, "output format")->check(CLI::IsMember{{"csv", "sql", "json"}});
    app.add_option("--distribution", options.distribution, "random number distribution")->check(CLI::IsMember{{"uniform", "normal"}});

    min_option->needs(max_option);
    max_option->needs(min_option);
    app.set_help_flag("-h,--help", "print this help");

    try {
        app.parse(argc, argv);
        if (options.min >= options.max) {
            throw CLI::ValidationError{"min must be smaller than max"};
        }
    } catch(const CLI::ParseError &e) {
        std::exit(app.exit(e));
    }

    return options;
}


int main(int argc, char* argv[]) {

    parse_cli_options(argc, argv);

    Settings settings{};
    
    auto data = generate_data(settings);
    (*settings.output)(data, settings, std::cout);
    return 0;
}
