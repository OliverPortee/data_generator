
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <map>

#include "CLI/CLI.hpp"
#include "data_generator/data_generator.hpp"

using namespace datagen;

/// @brief struct to hold all options that can be specified in command line interface
struct CliOptions {
    enum class OutputFormat { csv, sql, json };

    enum class RandomDistribution {
        /// every integer in specified range has same likelihood to be generated
        uniform,

        /// generates doubles in a normal distribution (Gaussian bell curve)
        normal,

        /// generates booleans in a bernoulli distribution (basically coin
        /// flipping with a specific probability to get heads)
        bernoulli
    };

    /// number of rows to be generated (cli: -n)
    unsigned int sample_count = 1000;

    /// number of columns to be generated (cli: -c)
    unsigned int col_count = 5;

    /// seed for random values (cli: --seed)
    unsigned int seed = std::random_device{}();

    /// output format (cli: -o, --output)
    OutputFormat output = OutputFormat::csv;

    /// tablename for sql output (cli: --tablename)
    std::string tablename = "table";

    /// random distribution (cli: subcommand)
    RandomDistribution distribution = RandomDistribution::uniform;

    /// minimum value to be potentially generated by uniform distribution (cli: --min)
    int min = std::numeric_limits<int>::min();

    /// maximum value to be potentially generated by uniform distribution (cli: --max)
    int max = std::numeric_limits<int>::max();

    /// mean value of normal distribution (cli: --mean)
    double mean = 0.0;

    /// standard deviation of normal distribution (cli: --stddev)
    double stddev = 1.0;

    /// probability p of bernoulli distribution (cli: -p);
    /// must be between 0 and 1
    double p = 0.5;
};

/// @brief creates the command line interface using CLI11 library
/// @param options stores the parsed values
/// @param[out] app output parameter (CLI::App's copy constructor is deleted)
void make_cli_interface(CliOptions& options, CLI::App& app) {
    app.require_subcommand(0, 1);

    app.add_option("-n", options.sample_count, "number of rows to be generated")->check(CLI::PositiveNumber);
    app.add_option("-c", options.col_count, "number of cols to be generated")->check(CLI::PositiveNumber);
    app.add_option("--seed", options.seed, "use seed for deterministic pseudo-random data");

    std::map<std::string, CliOptions::OutputFormat> map{
        {"csv", CliOptions::OutputFormat::csv},
        {"sql", CliOptions::OutputFormat::sql},
        {"json", CliOptions::OutputFormat::json}};
    app.add_option("-o,--output", options.output, "output format")->transform(CLI::CheckedTransformer{map, CLI::ignore_case});
    app.add_option("--tablename", options.tablename, "tablename for sql output");

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

/// @brief parse and validate the command line arguments
/// @param argc argc as received in main function
/// @param argv argv as received by main function
/// @returns CliOptions instance containing the parsed values
CliOptions parse_cli_options(int argc, char* argv[]) {
    CliOptions options{};
    CLI::App app{"random number generator"};
    make_cli_interface(options, app);

    try {
        app.parse(argc, argv);

        if (app.got_subcommand("uniform") && options.min >= options.max) {
                throw CLI::ValidationError{"min must be smaller than max"};
        }

        if (app.count("--tablename") && options.output != CliOptions::OutputFormat::sql) {
            throw CLI::ValidationError{"--tablename works only with --output sql"};
        }
        return options;
    } catch (const CLI::ParseError& e) {
        std::exit(app.exit(e));
    }
}

/// @brief utility function to select the right output method based on @see CliOptions::OutputFormat
/// @tparam T Type of the generated data (int, double, bool, ...)
template<class T>
void output(Data<T>&& data, const CliOptions& options) {
    switch(options.output) {
        case CliOptions::OutputFormat::csv:
            output_csv<T>(data, std::cout);
        break;
        case CliOptions::OutputFormat::json:
            output_json<T>(data, std::cout);
        break;
        case CliOptions::OutputFormat::sql:
            output_sql<T>(data, std::cout, options.tablename);
        break;
    }
}

/// @brief overloads << operator to print CliOptions instance
/// @details outputs a string that could be used to re-create the exact same random values
std::ostream& operator<<(std::ostream& os, const CliOptions& options) {
    switch (options.output) {
        case CliOptions::OutputFormat::json:
            os << "// ";  // actually there are no comments in json, but anyways
            break;
        case CliOptions::OutputFormat::csv:
            os << "# ";  // actually there are no comments in csv, but anyways
            break;
        case CliOptions::OutputFormat::sql:
            os << "-- ";
            break;
    }
    os << "gendata";
    os << " -n " << options.sample_count;
    os << " -c " << options.col_count;
    os << " --seed " << options.seed;
    os << " -o ";
    switch (options.output) {
        case CliOptions::OutputFormat::json:
            os << "json";
            break;
        case CliOptions::OutputFormat::csv:
            os << "csv";
            break;
        case CliOptions::OutputFormat::sql:
            os << "sql";
            os << " --tablename " << options.tablename;
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

/// @brief parses command line parameters, generates and outputs data (stdout)
/// and prints a string with used command line options (stderr)
int main(int argc, char* argv[]) {
    CliOptions options = parse_cli_options(argc, argv);

    switch (options.distribution) {
        case CliOptions::RandomDistribution::uniform: {
            std::uniform_int_distribution random{options.min, options.max};
            Settings settings{options.sample_count, options.col_count,
                              options.seed, random};
            output<int>(generate_data(settings), options);
        } break;
        case CliOptions::RandomDistribution::normal: {
            std::normal_distribution random{options.mean, options.stddev};
            Settings settings{options.sample_count, options.col_count,
                              options.seed, random};
            output<double>(generate_data(settings), options);
        } break;
        case CliOptions::RandomDistribution::bernoulli: {
            std::bernoulli_distribution random{options.p};
            Settings settings{options.sample_count, options.col_count,
                              options.seed, random};
            output<bool>(generate_data(settings), options);
        } break;
    }

    // std::cerr so that we can use shell pipes without problems
    std::cerr << options << std::endl;
}
