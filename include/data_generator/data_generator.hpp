
#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include <cassert>
#include <functional>
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

    RowView front() const { return RowView{data.begin(), col_count}; }

    RowView back() const { return (*this)[row_count - 1]; }

    void set_value(unsigned int row, unsigned int col, T&& value) {
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
            data.set_value(row, col, random(random_algo));
        }
    }
    return data;
}

/// @brief function template for outputting a single value (used for output
/// functions)
template <typename T>
using OutputFunction = std::function<void(const T&, std::ostream&)>;

namespace detail {

/// @brief simply output the given value
/// @details used as default argument for output functions
template <typename T>
void output_id(const T& t, std::ostream& ostream) {
    ostream << t;
}

/// @brief output function for a single value for json
/// @details the reason for this special function is that
/// specifically for json we need a template specialization
/// that prints "true" and "false" instead of 1 and 0;
template <typename T>
void output_id_json(const T& t, std::ostream& ostream) {
    ostream << t;
}

/// @brief template specialization for bool
/// @details json has boolean type with values "true" and "false"
template <>
void output_id_json<bool>(const bool& t, std::ostream& ostream) {
    ostream << (t ? "true" : "false");
}

}  // namespace detail

/// @brief output data to std::ostream in csv format
/// @tparam T type of the generated data
template <typename T>
void output_csv(const Data<T>& data, std::ostream& ostream,
                const OutputFunction<T>& o = detail::output_id<T>) {
    for (unsigned int row = 0; row < data.row_count - 1; ++row) {
        o(data[row][0], ostream);
        for (unsigned int col = 1; col < data.col_count; ++col) {
            ostream << ",";
            o(data[row][col], ostream);
        }
        ostream << "\n";
    }
    o(data[data.row_count - 1][0], ostream);
    for (unsigned int col = 1; col < data.col_count; ++col) {
        ostream << ",";
        o(data[data.row_count - 1][col], ostream);
    }
}

/// @brief output data to std::ostream in sql format
/// @tparam T Type of the generated data
/// @param tablename name of the table to insert the data into
template <typename T>
void output_sql(const Data<T>& data, std::ostream& ostream,
                const std::string& tablename,
                const OutputFunction<T>& o = detail::output_id<T>) {
    ostream << "INSERT INTO \"" << tablename << "\" VALUES\n";

    for (unsigned int row = 0; row < data.row_count - 1; ++row) {
        ostream << "  (";
        o(data[row][0], ostream);
        for (unsigned int col = 1; col < data.col_count; ++col) {
            ostream << ", ";
            o(data[row][col], ostream);
        }
        ostream << "),\n";
    }
    ostream << "  (";
    o(data[data.row_count - 1][0], ostream);
    for (unsigned int col = 1; col < data.col_count; ++col) {
        ostream << ", ";
        o(data[data.row_count - 1][col], ostream);
    }
    ostream << ");";
}

/// @brief output data to std::ostream in json format (nested array)
/// @tparam T Type of the generated data
template <typename T>
void output_json(const Data<T>& data, std::ostream& ostream,
                 const OutputFunction<T>& o = detail::output_id_json<T>) {
    ostream << "[\n";
    for (unsigned int row = 0; row < data.row_count - 1; ++row) {
        ostream << "  [";
        o(data[row][0], ostream);
        for (unsigned int col = 1; col < data.col_count; ++col) {
            ostream << ", ";
            o(data[row][col], ostream);
        }
        ostream << "],\n";
    }
    ostream << "  [";
    o(data[data.row_count - 1][0], ostream);
    for (unsigned int col = 1; col < data.col_count; ++col) {
        ostream << ", ";
        o(data[data.row_count - 1][col], ostream);
    }
    ostream << "]\n]";
}

}  // namespace datagen

#endif