
#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include <cassert>
#include <cstddef>
#include <functional>
#include <iterator>
#include <memory>
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
    const unsigned int row_count;
    const unsigned int col_count;

    Data(unsigned int row_count, unsigned int col_count)
        : row_count{row_count}, col_count{col_count} {
        assert(row_count > 0);
        assert(col_count > 0);
        data = std::vector<T>(row_count * col_count);
    }

    ConstRowIterator begin() const {
        return ConstRowIterator{data.data(), col_count};
    }

    ConstRowIterator end() const {
        return ConstRowIterator{data.data() + row_count * col_count, col_count};
    }

    std::size_t size() const { return row_count * col_count; }

    RowView operator[](std::size_t pos) const {
        return RowView{data.data() + col_count * pos, col_count};
    }

    void setValue(unsigned int row, unsigned int col, T&& value) {
        data[row * col_count + col] = value;
    }

   private:
    std::vector<T> data;

    struct RowView {
       private:
        struct ConstElementIterator;

       public:
        RowView(const T* ptr, std::size_t size)
            : ptr{ptr}, _size{size} {
            assert(ptr != nullptr);
        }
        const T* data() const { return ptr; }
        std::size_t size() const { return _size; }
        const T& operator[](std::size_t pos) const {
            return *(ptr + pos);
        }
        ConstElementIterator begin() const {
            return ConstElementIterator{ptr};
        }
        ConstElementIterator end() const {
            return ConstElementIterator{ptr + _size};
        }

       private:
        const T* ptr;
        std::size_t _size;

        struct ConstElementIterator {
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = const value_type*;
            using reference = const value_type&;

            ConstElementIterator(const T* ptr) : ptr{ptr} {
                assert(ptr != nullptr);
            }

            reference operator*() const { return *ptr; }
            pointer operator->() const { return ptr; }

            ConstElementIterator& operator++() {
                ++ptr;
                return *this;
            }
            ConstElementIterator operator++(int) {
                ConstElementIterator tmp = *this;
                ++(*this);
                return tmp;
            }

            ConstElementIterator& operator--() {
                --ptr;
                return *this;
            }
            ConstElementIterator operator--(int) {
                ConstElementIterator tmp = *this;
                --(*this);
                return tmp;
            }

            bool operator==(const ConstElementIterator& other) const {
                return ptr == other.ptr;
            }
            bool operator!=(const ConstElementIterator& other) const {
                return ptr != other.ptr;
            }

           private:
            const T* ptr;
        };
    };

    struct ConstRowIterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = RowView;
        using pointer = const value_type*;
        using reference = const value_type&;

        ConstRowIterator(const T* ptr, std::size_t size)
            : view{ptr, size} {
            assert(ptr != nullptr);
        }

        reference operator*() const { return view; }
        pointer operator->() const { return &view; }

        ConstRowIterator& operator++() {
            view = RowView{view.data() + view.size(), view.size()};
            return *this;
        }
        ConstRowIterator operator++(int) {
            ConstRowIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        ConstRowIterator& operator--() {
            view = RowView{view.data() - view.size(), view.size()};
            return *this;
        }

        ConstRowIterator operator--(int) {
            ConstRowIterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const ConstRowIterator& other) const {
            return view.size() == other.view.size() &&
                   view.data() == other.view.data();
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