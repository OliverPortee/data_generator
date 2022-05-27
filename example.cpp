#include "data_generator/data_generator.hpp"
#include <iostream>
#include <vector>

int main() {
    datagen::Data<int> data{10, 5};
    std::uniform_int_distribution h{};
    datagen::Data<int> data2 = datagen::generate_data(10, 5, std::move(h));

    datagen::output_csv(data2, std::cout);
}