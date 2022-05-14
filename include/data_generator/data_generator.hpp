
#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include <cassert>
#include <functional>
#include <ostream>
#include <random>
#include <vector>

namespace datagen {

/// @brief parameters for random number generation
///
/// @tparam RandomNumberDistribution see
/// https://en.cppreference.com/w/cpp/named_req/RandomNumberDistribution
template <class RandomNumberDistribution>
class Settings {
   public:
    using SeedType = std::random_device::result_type;

    /// @param sample_count number of rows to be generated
    /// @param col_count number of columns to be generated
    /// @param seed seed value for random number generation (same seed leads to
    /// same random values)
    /// @param random random number distribution such as
    /// uniform_int_distribution from stdlib; see
    /// https://en.cppreference.com/w/cpp/named_req/RandomNumberDistribution
    Settings(unsigned int sample_count, unsigned int col_count, SeedType seed,
             RandomNumberDistribution random)
        : sample_count{sample_count},
          col_count{col_count},
          seed{seed},
          random{random} {
        assert(sample_count > 0);
        assert(col_count > 0);
    }

   private:
    unsigned int sample_count;
    unsigned int col_count;
    SeedType seed;
    RandomNumberDistribution random;
};

/// @brief class to hold generated data
/// @tparam T Type of the data (int, double, bool, ...)
template <class T>
class Data {
   public:
    const unsigned int row_count;
    const unsigned int col_count;

    Data(unsigned int row_count, unsigned int col_count)
        : data(row_count * col_count),
          row_count{row_count},
          col_count{col_count} {
        assert(row_count > 0);
        assert(col_count > 0);
    }

    void set_value(unsigned int row, unsigned int col, T&& value) {
        data[row * col_count + col] = value;
    }

    T get_value(unsigned int row, unsigned int col) const {
        return data[row * col_count + col];
    }

   private:
    std::vector<T> data;
};

/// @brief function to do the actual work of generating the data
/// @tparam RandomNumberDistribution @see Settings
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

/// @brief output data to std::ostream in csv format
/// @tparam T Type of the generated data
template <class T>
void output_csv(const Data<T>& data, std::ostream& ostream) {
    for (unsigned int row = 0; row < data.row_count - 1; ++row) {
        ostream << data.get_value(row, 0);
        for (unsigned int col = 1; col < data.col_count; ++col) {
            ostream << "," << data.get_value(row, col);
        }
        ostream << "\n";
    }
    ostream << data.get_value(data.row_count - 1, 0);
    for (unsigned int col = 1; col < data.col_count; ++col) {
        ostream << "," << data.get_value(data.row_count - 1, col);
    }
    ostream << std::endl;
}

/// @brief output data to std::ostream in sql format
/// @tparam T Type of the generated data
/// @param tablename name of the table to insert the data into
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
    ostream << ");" << std::endl;
}

/// @brief output data to std::ostream in json format (nested array)
/// @tparam T Type of the generated data
template <class T>
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
    ostream << "]\n]" << std::endl;
}

}  // namespace datagen

#endif