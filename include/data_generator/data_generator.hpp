
#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include <functional>
#include <ostream>
#include <random>
#include <vector>

// see https://en.cppreference.com/w/cpp/named_req/RandomNumberDistribution
template <class RandomNumberDistribution>
struct Settings {
    unsigned int sample_count;
    unsigned int col_count;

    using SeedType = std::random_device::result_type;
    SeedType seed;
    RandomNumberDistribution random;

    Settings(unsigned int sample_count, unsigned int col_count, SeedType seed,
             RandomNumberDistribution random)
        : sample_count{sample_count},
          col_count{col_count},
          seed{seed},
          random{random} {}
};

template <class T>
class Data {
   private:
    std::vector<T> data;

   public:
    const unsigned int row_count;
    const unsigned int col_count;

    Data(unsigned int row_count, unsigned int col_count)
        : data(row_count * col_count),
          row_count{row_count},
          col_count{col_count} {}

    void set_value(unsigned int row, unsigned int col, T&& value) {
        data[row * col_count + col] = value;
    }

    T get_value(unsigned int row, unsigned int col) const {
        return data[row * col_count + col];
    }
};

template <class RandomNumberDistribution>
Data<typename RandomNumberDistribution::result_type> generate_data(
    Settings<RandomNumberDistribution>& settings) {
    std::mt19937 random_algo{settings.seed};
    using T = typename RandomNumberDistribution::result_type;
    Data<T> data{settings.sample_count, settings.col_count};
    for (unsigned int row = 0; row < settings.sample_count; ++row) {
        for (unsigned int col = 0; col < settings.col_count; ++col) {
            data.set_value(row, col, settings.random(random_algo));
        }
    }
    return data;
}

template <class T>
void output_csv(const Data<T>& data, std::ostream& ostream) {
    for (unsigned int row = 0; row < data.row_count; ++row) {
        ostream << data.get_value(row, 0);
        for (unsigned int col = 1; col < data.col_count; ++col) {
            ostream << "," << data.get_value(row, col);
        }
        ostream << "\n";
    }
}

template <class T>
void output_sql(const Data<T>& data, std::ostream& ostream,
                const std::string& tablename) {
    ostream << "INSERT INTO \"" << tablename << "\" VALUES\n";

    for (unsigned int row = 0; row < data.row_count - 1; ++row) {
        ostream << "  (" << data.get_value(row, 0);
        for (unsigned int col = 1; col < data.col_count; ++col) {
            ostream << ", " << data.get_value(row, col);
        }
        ostream << "),\n";
    }
    ostream << "  (" << data.get_value(data.row_count - 1, 0);
    for (unsigned int col = 1; col < data.col_count; ++col) {
        ostream << ", " << data.get_value(data.row_count - 1, col);
    }
    ostream << ");\n";
}

template<class T>
void output_json(const Data<T>& data, std::ostream& ostream) {
    ostream << "[\n";
    for (unsigned int row = 0; row < data.row_count - 1; ++row) {
        ostream << "  [" << data.get_value(row, 0);
        for (unsigned int col = 1; col < data.col_count; ++col) {
            ostream << ", " << data.get_value(row, col);
        }
        ostream << "],\n";
    }
    ostream << "  [" << data.get_value(data.row_count - 1, 0);
    for (unsigned int col = 1; col < data.col_count; ++col) {
        ostream << ", " << data.get_value(data.row_count - 1, col);
    }
    ostream << "]\n]\n";
}

#endif