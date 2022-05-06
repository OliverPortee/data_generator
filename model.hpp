
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

struct RandomDistribution {
    virtual std::string_view name() const = 0;
    virtual int nextInt() = 0;
};

struct Settings {
    std::optional<int> min;
    std::optional<int> max;
    unsigned int row_count;
    unsigned int sample_count;
    std::unique_ptr<OutputMethod> output;
    std::unique_ptr<RandomDistribution> random;

    Settings(std::optional<int> min, std::optional<int> max,
             unsigned int row_count, unsigned int sample_count,
             std::unique_ptr<OutputMethod> output,
             std::unique_ptr<RandomDistribution> random);
};

struct Data {
    std::vector<std::vector<int>> data;
    Data(unsigned int row_count, unsigned int sample_count);
};

struct UniformDistribution : RandomDistribution {
    UniformDistribution(int min, int max);
    UniformDistribution(unsigned int seed, int min, int max);

    std::string_view name() const;
    int nextInt();

   private:
    std::uniform_int_distribution<int> dist;
    std::mt19937 gen;
};

struct CsvOutput : OutputMethod {
    std::string_view name() const;
    void operator()(const Data& data, const Settings& settings,
                    std::ostream& ostream) const;
};

#endif