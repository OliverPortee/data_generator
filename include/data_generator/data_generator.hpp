
#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include <cassert>
#include <ostream>
#include <random>
#include <vector>

namespace datagen {

/// @brief class to hold generated data
/// @tparam T type of the data (int, double, bool, ...)
template <typename T>
class Data {
   private:
    struct ConstRowIterator;
    struct RowView;

   public:
    using Iterator = std::vector<T>::const_iterator;

    const unsigned int row_count;
    const unsigned int col_count;

    Data(unsigned int row_count, unsigned int col_count)
        : row_count{row_count}, col_count{col_count} {
        assert(row_count > 0);
        assert(col_count > 0);
        data = std::vector<T>(row_count * col_count);
    }

    ConstRowIterator begin() const {
        return ConstRowIterator{data.begin(), col_count};
    }

    ConstRowIterator end() const {
        return ConstRowIterator{data.end(), col_count};
    }

    unsigned int size() const { return row_count * col_count; }

    RowView operator[](unsigned int pos) const {
        return RowView{data.begin() + col_count * pos, col_count};
    }

    RowView front() const {
        return RowView{data.begin(), col_count};
    }

    RowView back() const {
        return (*this)[row_count - 1];
    }

    void setValue(unsigned int row, unsigned int col, T&& value) {
        data[row * col_count + col] = value;
    }

   private:
    std::vector<T> data;

    /// @brief provides an abstraction of a row in the data table
    struct RowView {
        friend ConstRowIterator;

        RowView(Iterator it, unsigned int size) : it{it}, _size{size} {}
        unsigned int size() const { return _size; }
        T operator[](unsigned int pos) const { return *(it + pos); }
        Iterator begin() const { return it; }
        Iterator end() const { return it + _size; }
        T front() const { return *it; }
        T back() const { return *(it + _size - 1); }

       private:
        Iterator it;
        unsigned int _size;
    };

    /// @brief iterator for iterating rows of the data table
    struct ConstRowIterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = long int;
        using value_type = RowView;
        using pointer = const value_type*;
        using reference = const value_type&;

        ConstRowIterator(Iterator it, unsigned int size) : view{it, size} {}

        reference operator*() const { return view; }
        pointer operator->() const { return &view; }

        ConstRowIterator& operator++() {
            view = RowView{view.it + view.size(), view.size()};
            return *this;
        }
        ConstRowIterator operator++(int) {
            ConstRowIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        ConstRowIterator& operator--() {
            view = RowView{view.it - view.size(), view.size()};
            return *this;
        }

        ConstRowIterator operator--(int) {
            ConstRowIterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const ConstRowIterator& other) const {
            return view.size() == other.view.size() && view.it == other.view.it;
        }

        bool operator!=(const ConstRowIterator& other) const {
            return !(*this == other);
        }

       private:
        RowView view;
    };
};

/// @brief function to do the actual work of generating the data
/// @tparam RandomNumberDistribution see
/// https://en.cppreference.com/w/cpp/named_req/RandomNumberDistribution
/// @param sample_count number of rows to be generated
/// @param col_count number of columns to be generated
/// @param random random number distribution such as
/// uniform_int_distribution from stdlib
/// @param seed seed value for random number generation (same seed leads
/// to same random values)
template <typename RandomNumberDistribution>
Data<typename RandomNumberDistribution::result_type> generate_data(
    unsigned int sample_count, unsigned int col_count,
    RandomNumberDistribution&& random,
    typename std::random_device::result_type seed = std::random_device{}()) {
    assert(sample_count > 0);
    assert(col_count > 0);
    std::mt19937 random_algo{seed};
    using T = typename RandomNumberDistribution::result_type;
    Data<T> data{sample_count, col_count};
    for (unsigned int row = 0; row < sample_count; ++row) {
        for (unsigned int col = 0; col < col_count; ++col) {
            data.setValue(row, col, random(random_algo));
        }
    }
    return data;
}

/// @brief output data to std::ostream in csv format
/// @tparam T Type of the generated data
template <typename T>
void output_csv(const Data<T>& data, std::ostream& ostream) {
    for (unsigned int row = 0; row < data.row_count - 1; ++row) {
        ostream << data[row][0];
        for (unsigned int col = 1; col < data.col_count; ++col) {
            ostream << "," << data[row][col];
        }
        ostream << "\n";
    }
    ostream << data[data.row_count - 1][0];
    for (unsigned int col = 1; col < data.col_count; ++col) {
        ostream << "," << data[data.row_count - 1][col];
    }
    ostream << std::endl;
}

/// @brief output data to std::ostream in sql format
/// @tparam T Type of the generated data
/// @param tablename name of the table to insert the data into
template <typename T>
void output_sql(const Data<T>& data, std::ostream& ostream,
                const std::string& tablename) {
    ostream << "INSERT INTO \"" << tablename << "\" VALUES\n";

    for (unsigned int row = 0; row < data.row_count - 1; ++row) {
        ostream << "  (" << data[row][0];
        for (unsigned int col = 1; col < data.col_count; ++col) {
            ostream << ", " << data[row][col];
        }
        ostream << "),\n";
    }
    ostream << "  (" << data[data.row_count - 1][0];
    for (unsigned int col = 1; col < data.col_count; ++col) {
        ostream << ", " << data[data.row_count - 1][col];
    }
    ostream << ");" << std::endl;
}

/// @brief output data to std::ostream in json format (nested array)
/// @tparam T Type of the generated data
template <typename T>
void output_json(const Data<T>& data, std::ostream& ostream) {
    ostream << "[\n";
    for (unsigned int row = 0; row < data.row_count - 1; ++row) {
        ostream << "  [" << data[row][0];
        for (unsigned int col = 1; col < data.col_count; ++col) {
            ostream << ", " << data[row][col];
        }
        ostream << "],\n";
    }
    ostream << "  [" << data[data.row_count - 1][0];
    for (unsigned int col = 1; col < data.col_count; ++col) {
        ostream << ", " << data[data.row_count - 1][col];
    }
    ostream << "]\n]" << std::endl;
}

}  // namespace datagen

#endif