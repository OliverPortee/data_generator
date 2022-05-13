
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
    unsigned int sample_count = 1000;            // -n
    unsigned int col_count = 5;                  // -c
    unsigned int seed = std::random_device{}();  // --seed
    std::string output = "csv";                  // -o
    std::string distribution = "uniform";        // --distribution
};

CliOptions parse_cli_options(int argc, char* argv[]) {
    CliOptions options{};
    CLI::App app{"random number generator"};
    app.add_option("-n", options.sample_count, "number of rows to be generated");
    app.add_option("-c", options.col_count, "number of cols to be generated");
    app.add_option("--seed", options.seed, "use seed for deterministic pseudo-random data");
    app.add_option("-o,--ouput", options.output, "output format")->check(CLI::IsMember{{"csv", "sql", "json"}});

    auto uniform_command = app.add_subcommand("uniform", "generates random integers from a uniform distribution");
    auto min_option = uniform_command->add_option("--min", "minimum value of random ints");
    auto max_option = uniform_command->add_option("--max", "maximum value of random ints");
    min_option->needs(max_option)->default_val<int>(std::numeric_limits<int>::min());
    max_option->needs(min_option)->default_val<int>(std::numeric_limits<int>::max());

    auto normal_command = app.add_subcommand("normal", "generates random doubles from a normal distribution");
    normal_command->add_option("--mean", "the mean of the normal distribution")->default_val<double>(0.0);
    normal_command->add_option("--stddev", "the standard deviation of the normal distribution")->default_val<double>(1.0);

    auto bernoulli_command = app.add_subcommand("bernoulli", "generates random booleans from a bernoulli distribution");
    bernoulli_command->add_option("-p", "the probability p of the bernoulli distribution")->default_val<double>(0.5);

    try {
        app.parse(argc, argv);
        // TODO: check that min is smaller than max
    } catch(const CLI::ParseError &e) {
        std::exit(app.exit(e));
    }

    return options;
}


int main(int argc, char* argv[]) {

    parse_cli_options(argc, argv);

    std::bernoulli_distribution n{};

    Settings settings{};
    
    auto data = generate_data(settings);
    (*settings.output)(data, settings, std::cout);
    return 0;
}
