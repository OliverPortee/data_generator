
#include <cstring>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <sstream>
#include <string>

#include "CLI/CLI.hpp"
#include "model.hpp"

struct CliOptions {
    unsigned int sample_count = 1000;            // -n
    unsigned int col_count = 5;                  // -c
    unsigned int seed = std::random_device{}();  // --seed
    std::string output = "csv";                  // -o
};

CliOptions parse_cli_options(int argc, char* argv[]) {
    CliOptions options{};
    CLI::App app{"random number generator"};
    app.add_option("-n", options.sample_count, "number of rows to be generated");
    app.add_option("-c", options.col_count, "number of cols to be generated");
    app.add_option("--seed", options.seed, "use seed for deterministic pseudo-random data");
    app.add_option("-o,--ouput", options.output, "output format")->check(CLI::IsMember{{"csv", "sql", "json"}});

    auto uniform_command = app.add_subcommand( "uniform", "generates random integers from a uniform distribution");
    auto min_option = uniform_command->add_option("--min", "minimum value of random ints");
    auto max_option = uniform_command->add_option("--max", "maximum value of random ints");
    min_option->needs(max_option) ->default_val<int>(std::numeric_limits<int>::min());
    max_option->needs(min_option) ->default_val<int>(std::numeric_limits<int>::max());

    auto normal_command = app.add_subcommand( "normal", "generates random doubles from a normal distribution");
    auto mean_option = normal_command->add_option("--mean", "the mean of the normal distribution")->default_val<double>(0.0);
    auto stddev_option = normal_command ->add_option("--stddev", "the standard deviation of the normal distribution")->default_val<double>(1.0);

    auto bernoulli_command = app.add_subcommand( "bernoulli", "generates random booleans from a bernoulli distribution");
    auto p_option = bernoulli_command ->add_option("-p", "the probability p of the bernoulli distribution")->default_val<double>(0.5)->check(CLI::Range{0.0, 1.0});

    try {
        app.parse(argc, argv);

        if (app.got_subcommand(uniform_command)) {
            if (min_option->as<int>() >= max_option->as<int>()) {
                throw CLI::ValidationError{"min must be smaller than max"};
            }

            std::uniform_int_distribution<int> random{min_option->as<int>(), max_option->as<int>()};
            Settings settings{options.sample_count, options.col_count, options.seed, random};
            output_csv<int>(generate_data(settings), std::cout);
        } else if (app.got_subcommand(normal_command)) {
            std::normal_distribution random{mean_option->as<double>(), stddev_option->as<double>()};
            Settings settings{options.sample_count, options.col_count, options.seed, random};
            output_csv<double>(generate_data(settings), std::cout);
        } else if (app.got_subcommand(bernoulli_command)) {
            std::bernoulli_distribution random{p_option->as<double>()};
            Settings settings{options.sample_count, options.col_count, options.seed, random};
            output_csv<bool>(generate_data(settings), std::cout);
        }
    } catch (const CLI::ParseError& e) {
        std::exit(app.exit(e));
    }

    return options;
}

int main(int argc, char* argv[]) {
    parse_cli_options(argc, argv);
}
