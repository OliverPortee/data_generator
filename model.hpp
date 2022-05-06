
#include <optional>
#include <vector>
#include <string_view>
#include <memory>
#include <functional>
#include <ostream>

struct Data;
struct Settings;

struct OutputMethod {
    virtual std::string_view name() const = 0;
    virtual void operator()(const Data& data, const Settings& settings, std::ostream& ostream) const = 0;
};

struct RandomDistribution {
    virtual std::string_view name() const = 0;
    virtual int nextInt() = 0;
};

struct Settings {
    std::optional<int> min;
    std::optional<int> max;
    unsigned int row_count = 5;
    unsigned int sample_count = 1000;
    std::unique_ptr<OutputMethod> output;
    std::unique_ptr<RandomDistribution> random;
};

struct Data {
    std::vector<std::vector<int>> data;
    Data(unsigned int row_count, unsigned int sample_count);
};