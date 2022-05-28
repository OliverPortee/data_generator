#include "data_generator/data_generator.hpp"
#include <iostream>
#include <vector>

int main() {
    datagen::Data<int> data{10, 5};
    std::uniform_int_distribution h{};
    datagen::Data<int> data2 = datagen::generate_data(10, 5, std::move(h));

    auto row1 = data2[1];
    auto b1 = row1.begin();
    row1.begin()++;
    auto b2 = row1.begin();
    std::cout << (b1 == b2) << std::endl;

    datagen::output_csv(data2, std::cout);
}