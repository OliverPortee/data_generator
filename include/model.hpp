
#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include <functional>
#include <ostream>
#include <random>
#include <vector>

template <class T>
struct Data;

template <class T>
using OutputMethod =
    std::function<void(const Data<T>& data, std::ostream& ostream)>;

// see https://en.cppreference.com/w/cpp/named_req/RandomNumberDistribution
template <class RandomNumberDistribution>
struct Settings {
    unsigned int col_count;
    unsigned int sample_count;

    using SeedType = std::random_device::result_type;
    SeedType seed;
    OutputMethod<typename RandomNumberDistribution::result_type> output;
    RandomNumberDistribution random;

    Settings(unsigned int col_count, unsigned int sample_count, SeedType seed,
             OutputMethod<typename RandomNumberDistribution::result_type> output,
             RandomNumberDistribution random)
        : col_count{col_count},
          sample_count{sample_count},
          seed{seed},
          output{output},
          random{random} {}
};

template <class T>
struct Data {
    std::vector<std::vector<T>> data;
    Data(unsigned int col_count, unsigned int sample_count)
        : data(sample_count, std::vector(col_count, T{})) {}
};

template <class RandomNumberDistribution>
Data<typename RandomNumberDistribution::result_type> generate_data(
    Settings<RandomNumberDistribution>& settings) {
    std::mt19937 random_algo{settings.seed};
    Data<typename RandomNumberDistribution::result_type> data{settings.col_count, settings.sample_count};
    for (unsigned int i = 0; i < settings.sample_count; ++i) {
        for (unsigned int j = 0; j < settings.col_count; ++j) {
            data.data[i][j] = settings.random(random_algo);
        }
    }
    return data;
}

template <class T>
OutputMethod<T> output_csv = [](const Data<T>& data, std::ostream& ostream) {
    for (auto& row : data.data) {
        ostream << row.front();
        for (unsigned int i = 1; i < row.size(); ++i) {
            ostream << "," << row[i];
        }
        ostream << "\n";
    }
};

#endif