
#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include <functional>
#include <memory>
#include <optional>
#include <ostream>
#include <random>
#include <string_view>
#include <vector>

struct Data;
struct Settings;

struct OutputMethod {
    virtual std::string_view name() const = 0;
    virtual void operator()(const Data& data, const Settings& settings,
                            std::ostream& ostream) const = 0;
};

struct CsvOutput : OutputMethod {
    std::string_view name() const;
    void operator()(const Data& data, const Settings& settings,
                    std::ostream& ostream) const;
};

struct RandomDistribution {
    virtual std::string_view name() const = 0;
    virtual int nextInt() = 0;
};

struct UniformDistribution : RandomDistribution {
    UniformDistribution();
    UniformDistribution(unsigned int seed);
    UniformDistribution(int min, int max);
    UniformDistribution(unsigned int seed, int min, int max);

    std::string_view name() const;
    int nextInt();

   private:
    UniformDistribution(std::uniform_int_distribution<int> dist,
                        std::mt19937 gen = std::mt19937{
                            std::random_device{}()});

    std::uniform_int_distribution<int> dist;
    std::mt19937 gen;
};

struct Settings {
    unsigned int col_count;
    unsigned int sample_count;
    std::unique_ptr<OutputMethod> output;
    std::unique_ptr<RandomDistribution> random;

    Settings(
        unsigned int col_count = 5, unsigned int sample_count = 1000,
        std::unique_ptr<OutputMethod> output = std::make_unique<CsvOutput>(),
        std::unique_ptr<RandomDistribution> random =
            std::make_unique<UniformDistribution>());
};

struct Data {
    std::vector<std::vector<int>> data;
    Data(unsigned int col_count, unsigned int sample_count);
};

#endif