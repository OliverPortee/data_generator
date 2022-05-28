#include <algorithm>
#include <complex>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#include "data_generator/data_generator.hpp"

// example on how to implement your own random number distribution;
// this implementation does not fulfill all requirements of a random number
// distribution but that's all we need
struct MyOwnRandomNumberDistribution {
    // define the type of your random numbers
    using result_type = std::complex<long unsigned int>;

    template <std::uniform_random_bit_generator G>
    result_type operator()(G& gen) {
        return std::complex<long unsigned int>{gen(), gen()};
    }
};

int main() {
    // use a random distribution from stdlib (or implement your own)
    // see https://en.cppreference.com/w/cpp/named_req/RandomNumberDistribution
    std::uniform_int_distribution h{-6, 99};
    datagen::Data<int> data = datagen::generate_data(10, 5, std::move(h));

    // easily write the data into a file
    std::ofstream ofs{"testfile.csv"};
    datagen::output_csv(data, ofs);
    ofs.close();

    // you can also use your own random number generator
    auto complex_data =
        datagen::generate_data(5, 3, MyOwnRandomNumberDistribution{});
    datagen::output_json(complex_data, std::cout);

    // it's possible to iterate through rows and columns...
    for (const auto& row : data) {
        for (const int& random_number : row) {
            // process your random_number
            (void)random_number;
        }
    }

    // ...or to access elements with []
    data[2][1];

    // stdlib function work as well
    std::for_each_n(data.front().begin(), data.size(), [](auto) {
        // process your random number
    });

    auto even_count = std::count_if(data.front().begin(), data.back().end(), [](const int& i) {
        return i % 2 == 0;
    });
    std::cout << even_count << std::endl;

}