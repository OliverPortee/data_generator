#include "data_generator/data_generator.hpp"
#include <catch2/catch_test_macros.hpp>
#include <limits>

TEST_CASE("Data tests", "") {
    datagen::Data<int> data{20, 4};
    for (auto row : data) {
        REQUIRE(row.size() == data.col_count);
        for (unsigned int col = 0; col < data.col_count; ++col) {
            REQUIRE(row[col] == 0);
        }
        for (auto elem : row) {
            REQUIRE(elem == 0);
        }
    }

    data.setValue(3, 2, -8);
    REQUIRE(data[3][2] == -8);

    data.setValue(0, 0, std::numeric_limits<int>::min());
    REQUIRE(data[0][0] == std::numeric_limits<int>::min());

    data.setValue(data.row_count - 1, data.col_count - 1, std::numeric_limits<int>::max());
    REQUIRE(data[data.row_count - 1][data.col_count - 1] == std::numeric_limits<int>::max());
}