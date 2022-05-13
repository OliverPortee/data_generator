
#include <cxxopts.hpp>
#include <iostream>

#include "data_generation.hpp"

Settings parse_cli_parameters(int argc, char* argv[]) {
    // --min NUMBER
    // --max NUMBER
    // -n COUNT # default 1000
    // -c COUNT # default 5
    // --seed NUMBER
    // -o csv|sql # default csv
    // --distribution uniform|normal # default uniform

    cxxopts::Options options("data generator", "generate random integer data");
    options.add_options()("min", "minimum of random values",
                          cxxopts::value<int>())(
        "max", "maximum of random values",
        cxxopts::value<
            int>())("n,samplecount", "number of rows to be generated",
                    cxxopts::value<
                        unsigned int>())("a,along", "bla",
                                         cxxopts::value<
                                             bool>())("b,blong", "bla",
                                                      cxxopts::value<bool>());
    auto result = options.parse(argc, argv);

    // TODO: --help
    // TODO: catch parse errors

    bool has_min = result.count("min");
    bool has_max = result.count("max");
    if (has_min && !has_max) {
        std::cerr << "must specify --max when using --min" << std::endl;
        std::exit(0);
    }
    if (has_max && !has_min) {
        std::cerr << "must specify --min when using --max" << std::endl;
        std::exit(0);
    }
    if (has_min && has_max) {
        if (result["min"].as<int>() >= result["max"].as<int>()) {
            std::cerr << "minimum value must be smaller than maximum value"
                      << std::endl;
        }
    }

    std::cout << result.count("a") << std::endl;
    std::cout << result.count("b") << std::endl;

    return Settings{0,
                    1000000,
                    5,
                    10,
                    std::make_unique<CsvOutput>(),
                    std::make_unique<UniformDistribution>(0, 1000000)};
}

int main(int argc, char* argv[]) {
    auto settings = parse_cli_parameters(argc, argv);
    auto data = generate_data(settings);
    (*settings.output)(data, settings, std::cout);
    return 0;
}
