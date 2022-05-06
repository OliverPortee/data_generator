
#include <optional>
#include <vector>
#include <string_view>
#include <memory>
#include <functional>

struct OutputMethod {
    virtual std::string_view name() = 0;
    virtual void operator()();
};

struct RandomDistribution {
    std::function<int()> nextInt;
    std::string name;
};

struct Settings {
    std::optional<int> min;
    std::optional<int> max;
    unsigned int row_count = 5;
    unsigned int sample_count = 1000;
    std::unique_ptr<OutputMethod> output;
    RandomDistribution random;
};

struct Data {
    std::vector<std::vector<int>> data;
    Data(unsigned int row_count, unsigned int sample_count);
};