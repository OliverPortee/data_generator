
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <map>

#include "CLI/CLI.hpp"
#include "data_generator/data_generator.hpp"

using namespace datagen;

struct CliOptions {
    enum class OutputType { csv, sql, json };
    enum class RandomDistribution { uniform, normal, bernoulli };

    unsigned int sample_count = 1000;            // -n
    unsigned int col_count = 5;                  // -c
    unsigned int seed = std::random_device{}();  // --seed
    OutputType output = OutputType::csv;         // -o,--output
    RandomDistribution distribution = RandomDistribution::uniform;
    int min = std::numeric_limits<int>::min();   // --min (uniform dist)
    int max = std::numeric_limits<int>::max();   // --max (uniform dist)
    double mean = 0.0;                           // --mean (normal dist)
    double stddev = 1.0;                         // --stddev (normal dist)
    double p = 0.5;                              // -p (bernoulli dist)
};

void make_cli_interface(CliOptions& options, CLI::App& app) {

    app.require_subcommand(0, 1);

    app.add_option("-n", options.sample_count, "number of rows to be generated")->check(CLI::PositiveNumber);
    app.add_option("-c", options.col_count, "number of cols to be generated")->check(CLI::PositiveNumber);
    app.add_option("--seed", options.seed, "use seed for deterministic pseudo-random data");

    std::map<std::string, CliOptions::OutputType> map{
        {"csv", CliOptions::OutputType::csv},
        {"sql", CliOptions::OutputType::sql},
        {"json", CliOptions::OutputType::json}};
    app.add_option("-o,--ouput", options.output, "output format")->transform(CLI::CheckedTransformer{map, CLI::ignore_case});

    auto uniform_command = app.add_subcommand("uniform", "generates random integers from a uniform distribution")
        ->callback([&]() {
        options.distribution = CliOptions::RandomDistribution::uniform;
    });
    uniform_command->add_option("--min", options.min, "minimum value of random ints");
    uniform_command->add_option("--max", options.max, "maximum value of random ints");

    auto normal_command = app.add_subcommand("normal", "generates random doubles from a normal distribution")
        ->callback([&]() {
        options.distribution = CliOptions::RandomDistribution::normal;
    });
    normal_command->add_option("--mean", options.mean, "the mean of the normal distribution");
    normal_command->add_option("--stddev", options.stddev, "the standard deviation of the normal distribution");

    auto bernoulli_command = app.add_subcommand("bernoulli", "generates random booleans from a bernoulli distribution")
        ->callback([&]() {
        options.distribution = CliOptions::RandomDistribution::bernoulli;
    });
    bernoulli_command->add_option("-p", options.p, "the probability p of the bernoulli distribution")->check(CLI::Range{0.0, 1.0});
}

CliOptions parse_cli_options(int argc, char* argv[]) {
    CliOptions options{};
    CLI::App app{"random number generator"};
    make_cli_interface(options, app);

    try {
        app.parse(argc, argv);

        if (app.got_subcommand("uniform") && options.min >= options.max) {
                throw CLI::ValidationError{"min must be smaller than max"};
        }
        return options;
    } catch (const CLI::ParseError& e) {
        std::exit(app.exit(e));
    }
}

template<class T>
void output(CliOptions::OutputType o, Data<T>&& data) {
    switch(o) {
        case CliOptions::OutputType::csv:
            output_csv<T>(data, std::cout);
        break;
        case CliOptions::OutputType::json:
            output_json<T>(data, std::cout);
        break;
        case CliOptions::OutputType::sql:
            std::string table_name{"table"};
            output_sql<T>(data, std::cout, table_name);
        break;
    }
}

std::ostream& operator<<(std::ostream& os, const CliOptions& options) {
    switch (options.output) {
        case CliOptions::OutputType::json:
            os << "// ";  // actually there are no comments in json, but anyways
            break;
        case CliOptions::OutputType::csv:
            os << "# ";  // actually there are no comments in csv, but anyways
            break;
        case CliOptions::OutputType::sql:
            os << "-- ";
            break;
    }
    os << "gendata";
    os << " -n " << options.sample_count;
    os << " -c " << options.col_count;
    os << " --seed " << options.seed;
    os << " -o ";
    switch (options.output) {
        case CliOptions::OutputType::json:
            os << "json";
            break;
        case CliOptions::OutputType::csv:
            os << "csv";
            break;
        case CliOptions::OutputType::sql:
            os << "sql";
            break;
    }

    switch (options.distribution) {
        case CliOptions::RandomDistribution::uniform:
            os << " uniform";
            os << " --min " << options.min;
            os << " --max " << options.max;
            break;
        case CliOptions::RandomDistribution::normal:
            os << " normal";
            os << " --mean " << options.mean;
            os << " --stddev " << options.stddev;
            break;
        case CliOptions::RandomDistribution::bernoulli:
            os << " bernoulli";
            os << " -p " << options.p;
            break;
    }
    return os;
}

int main(int argc, char* argv[]) {
    CliOptions options = parse_cli_options(argc, argv);

    switch (options.distribution) {
        case CliOptions::RandomDistribution::uniform: {
            std::uniform_int_distribution random{options.min, options.max};
            Settings settings{options.sample_count, options.col_count,
                              options.seed, random};
            output<int>(options.output, generate_data(settings));
        } break;
        case CliOptions::RandomDistribution::normal: {
            std::normal_distribution random{options.mean, options.stddev};
            Settings settings{options.sample_count, options.col_count,
                              options.seed, random};
            output<double>(options.output, generate_data(settings));
        } break;
        case CliOptions::RandomDistribution::bernoulli: {
            std::bernoulli_distribution random{options.p};
            Settings settings{options.sample_count, options.col_count,
                              options.seed, random};
            output<bool>(options.output, generate_data(settings));
        } break;
    }

    // std::cerr so that we can use shell pipes without problems
    std::cerr << options << std::endl;
}
