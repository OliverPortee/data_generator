
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

UniformDistribution::UniformDistribution(unsigned int seed, int min, int max)
    : gen{seed}, dist{min, max} {}

UniformDistribution::UniformDistribution(int min, int max)
    : UniformDistribution{std::random_device{}(), min, max} {}

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