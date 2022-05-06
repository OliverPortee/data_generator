
#include "do_it.hpp"
#include <iostream>

void do_it(const Settings& settings) {
    Data data{settings.row_count, settings.sample_count};
    for (unsigned int i = 0; i < settings.sample_count; ++i) {
        for (unsigned int j = 0; j < settings.row_count; ++j) {
            data.data[i][j] = settings.random->nextInt();
        }
    }
    
    settings.output->operator()(data, settings, std::cout); // fail
}