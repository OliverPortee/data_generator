
#include "data_generation.hpp"
#include <iostream>

Data generate_data(const Settings& settings) {
    Data data{settings.col_count, settings.sample_count};
    for (unsigned int i = 0; i < settings.sample_count; ++i) {
        for (unsigned int j = 0; j < settings.col_count; ++j) {
            data.data[i][j] = settings.random->nextInt();
        }
    }
    return data;
}