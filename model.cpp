
#include "model.hpp"

#include <cassert>

Settings::Settings(std::optional<int> min, std::optional<int> max,
                   unsigned int row_count, unsigned int sample_count,
                   std::unique_ptr<OutputMethod> output,
                   std::unique_ptr<RandomDistribution> random)
    : min{min},
      max{max},
      row_count{row_count},
      sample_count{sample_count},
      output{std::move(output)},
      random{std::move(random)} {}

Data::Data(unsigned int row_count, unsigned int sample_count)
    : data(sample_count, std::vector<int>(row_count, 0)) {}

UniformDistribution::UniformDistribution(
    std::uniform_int_distribution<int> dist,
    std::mt19937 gen = std::mt19937{std::random_device{}()})
    : dist{dist}, gen{gen} {}

UniformDistribution::UniformDistribution()
    : UniformDistribution{std::uniform_int_distribution<int>{}} {}

UniformDistribution UniformDistribution::fromSeed(unsigned int seed) {
    return UniformDistribution{std::uniform_int_distribution<int>{},
                               std::mt19937{seed}};
}

UniformDistribution UniformDistribution::fromMinMax(int min, int max) {
    return UniformDistribution{std::uniform_int_distribution<int>{min, max}};
}

UniformDistribution UniformDistribution::fromSeedMinMax(unsigned int seed,
                                                        int min, int max) {
    return UniformDistribution {
        std::uniform_int_distribution<int>{min, max}, std::mt19937 { seed }
    };
}

std::string_view UniformDistribution::name() const {
    return "UniformDistribution";
}

int UniformDistribution::nextInt() { return dist(gen); }

std::string_view CsvOutput::name() const { return "csv"; }

void CsvOutput::operator()(const Data& data, const Settings& settings,
                           std::ostream& ostream) const {
    for (auto& row : data.data) {
        ostream << row.front();
        for (unsigned int i = 1; i < row.size(); ++i) {
            ostream << "," << row[i];
        }
        ostream << "\n";
    }
}