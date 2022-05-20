#include "data_generator/data_generator.hpp"
#include <iostream>

int main() {
    datagen::Data<int> data{10, 5};
    for (auto& row : data) {
        for (auto& elem : row) {
            elem = 5;
        }
    }

    for (auto& row : data) {
        std::cout << row.size() << std::endl;
    }
}