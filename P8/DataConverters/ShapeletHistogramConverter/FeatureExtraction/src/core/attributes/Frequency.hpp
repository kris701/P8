#ifndef FEATUREEXTRACTION_FREQUENCY_H
#define FEATUREEXTRACTION_FREQUENCY_H

#include <string>
#include "Attribute.hpp"

class Frequency : public Attribute {
public:
    explicit Frequency(double tolerance) : tolerance(tolerance) {}

    [[nodiscard]] inline std::string Name() const final { return "Frequency"; };
    [[nodiscard]] inline std::string Param1() const final { return std::to_string(tolerance); };

    [[nodiscard]] bool ToleranceMatch(const Series &series, uint offset, const Series &window) const {
        const double tempTolerance = tolerance + series[offset];

        for (uint i = 1; i < window.size(); i++)
            if (std::abs(series[i + offset] - window.at(i)) > tempTolerance)
                return false;

        return true;
    }

    [[nodiscard]] double GenerateValue(const Series &series, const Series &window) const final {
        uint checked = 0;
        uint matchCount = 0;

        for (uint i = 0; i < series.size() - window.size(); ++i) {
            if (ToleranceMatch(series, i, window))
                ++matchCount;
            ++checked;
        }

        return (double) matchCount / checked;
    }

private:
    const double tolerance;
};

#endif //FEATUREEXTRACTION_FREQUENCY_H
