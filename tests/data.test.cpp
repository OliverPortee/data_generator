#include "data_generator/data_generator.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace datagen;

TEST_CASE("Data class interface is correct", "[Data]") {
    Data<int> di{5, 7}; // 5 rows, 7 columns
    Data<double> dd{99, 3};
    Data<bool> db{1, 66};

    SECTION("col_count and row_count") {
        CHECK(di.row_count == 5);
        CHECK(di.col_count == 7);
        CHECK(dd.row_count == 99);
        CHECK(dd.col_count == 3);
        CHECK(db.row_count == 1);
        CHECK(db.col_count == 66);
    }

    // setting some values
    int value = 0;
    for (unsigned int row = 0; row < di.row_count; ++row) {
        for (unsigned int col = 0; col < di.col_count; ++col) {
            di.set_value(row, col, value++);
        }
    }

    SECTION("set_value, operator[]") {
        int value = 0;
        for (unsigned int row = 0; row < di.row_count; ++row) {
            for (unsigned int col = 0; col < di.col_count; ++col) {
                CHECK(di[row][col] == value++);
            }
        }
    }

    SECTION("begin and end") {
        // no infinite loop
        for (auto it = di.begin(); it != di.end(); ++it) {}
        for (auto it = dd.begin(); it != dd.end(); ++it) {}
        for (auto it = db.begin(); it != db.end(); ++it) {}

        CHECK((*di.begin()).front() == di[0][0]);
        auto end = di.end();
        --end;
        CHECK((*end)[0] == 28);
        CHECK((*end)[6] == 34);
    }

    SECTION("size") {
        CHECK(di.size() == 5 * 7);
        CHECK(dd.size() == 99 * 3);
        CHECK(db.size() == 1 * 66);
    }

    SECTION("RowView") {
        for (unsigned int row = 0; row < di.row_count; ++row) {
            CHECK(di[row].size() == di.col_count);
        }
        CHECK(di[0].front() == 0);
        CHECK(di[0].back() == 6);
        CHECK(di[4].front() == 28);
        CHECK(di[4].back() == 34);

        auto row = di[1];
        int value = 7;
        for (auto it = row.begin(); it != row.end(); ++it) {
            CHECK(*it == value++);
        }
        CHECK(row[0] == 7);
        CHECK(row[6] == 13);
    }

    SECTION("ConstRowIterator") {
        CHECK(di.begin()->size() == di.col_count);
        CHECK(di.begin()->front() == di[0][0]);
        CHECK((*di.begin())[0] == di[0][0]);
        CHECK(dd.begin() != dd.end());
        CHECK(dd.begin() == dd.begin());
        auto b = dd.begin();
        b++; b--;
        CHECK(b == dd.begin());
        auto c = dd.begin();
        ++c; --c;
        CHECK(c == dd.begin());
        CHECK(c++ == dd.begin());
        --c;
        CHECK(c-- == dd.begin());
        CHECK(c != dd.begin());
    }
}