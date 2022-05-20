
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
/// @tparam T Type of the data (int, double, bool, ...)
template <typename T>
class Data {
   private:
    struct RowView {
       private:
        struct ElementIterator;

       public:
        constexpr RowView(T* ptr, std::size_t size) : ptr{ptr}, _size{size} {
            assert(ptr != nullptr);
        }

        constexpr T* data() const { return ptr; }
        constexpr std::size_t size() const { return _size; }
        constexpr T& operator[](std::size_t pos) { return *(ptr + pos); }
        constexpr ElementIterator begin() { return ElementIterator{ptr}; }
        constexpr ElementIterator end() { return ElementIterator{ptr + _size}; }

       private:
        T* ptr;
        std::size_t _size;

        struct ElementIterator {
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;  // TODO
            using value_type = T;
            using pointer = value_type*;
            using reference = value_type&;

            ElementIterator(T* ptr) : ptr{ptr} { assert(ptr != nullptr); }

            reference operator*() const { return *ptr; }
            pointer operator->() const { return ptr; }

            ElementIterator& operator++() {
                ++ptr;
                return *this;
            }
            ElementIterator operator++(int) {
                ElementIterator tmp = *this;
                ++(*this);
                return tmp;
            }

            ElementIterator& operator--() {
                --ptr;
                return *this;
            }
            ElementIterator operator--(int) {
                ElementIterator tmp = *this;
                --(*this);
                return tmp;
            }

            bool operator==(const ElementIterator& other) const {
                return ptr == other.ptr;
            }
            bool operator!=(const ElementIterator& other) const {
                return ptr != other.ptr;
            }

           private:
            T* ptr;
        };
    };

    struct RowIterator {
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;  // TODO
        using value_type = RowView;
        using pointer = value_type*;
        using reference = value_type&;

        RowIterator(T* ptr, std::size_t size) : view{ptr, size} {
            assert(ptr != nullptr);
        }

        reference operator*() { return view; }
        pointer operator->() const { return &view; }

        RowIterator& operator++() {
            view = RowView{view.data() + view.size(), view.size()};
            return *this;
        }
        RowIterator operator++(int) {
            RowIterator tmp = *this;
            ++(*this);
            return tmp;
        }

        RowIterator& operator--() {
            view = RowView{view.data() - view.size(), view.size()};
            return *this;
        }

        RowIterator operator--(int) {
            RowIterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const RowIterator& other) const {
            return view.size() == other.view.size() &&
                   view.data() == other.view.data();
        }

        bool operator!=(const RowIterator& other) const {
            return !(*this == other);
        }

       private:
        RowView view;
    };

   public:
    const unsigned int row_count;
    const unsigned int col_count;

    Data(unsigned int row_count, unsigned int col_count)
        : row_count{row_count}, col_count{col_count} {
        assert(row_count > 0);
        assert(col_count > 0);
        data = std::make_unique<T[]>(row_count * col_count);
        assert(data != nullptr);
    }

    RowIterator begin() { return RowIterator{data.get(), col_count}; }

    RowIterator end() {
        return RowIterator{data.get() + row_count * col_count, col_count};
    }

   private:
    std::unique_ptr<T[]> data;


};

/// @brief function to do the actual work of generating the data
/// @tparam RandomNumberDistribution see
/// https://en.cppreference.com/w/cpp/named_req/RandomNumberDistribution
/// @param sample_count number of rows to be generated
/// @param col_count number of columns to be generated
/// @param seed seed value for random number generation (same seed leads
/// to same random values)
/// @param random random number distribution such as
/// uniform_int_distribution from stdlib
template <typename RandomNumberDistribution>
Data<typename RandomNumberDistribution::result_type> generate_data(
    unsigned int sample_count, unsigned int col_count,
    typename std::random_device::result_type seed,
    RandomNumberDistribution random) {
    assert(sample_count > 0);
    assert(col_count > 0);
    std::mt19937 random_algo{seed};
    using T = typename RandomNumberDistribution::result_type;
    Data<T> data{sample_count, col_count};
    for (unsigned int row = 0; row < sample_count; ++row) {
        for (unsigned int col = 0; col < col_count; ++col) {
            data[row][col] = random(random_algo);
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