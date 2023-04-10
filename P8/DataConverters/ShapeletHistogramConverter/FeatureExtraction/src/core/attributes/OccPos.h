#ifndef FEATUREEXTRACTION_OCCPOS_H
#define FEATUREEXTRACTION_OCCPOS_H

#include <string>
#include "Attribute.h"

class OccPos : public Attribute {
public:
    explicit OccPos(double tolerance) : tolerance(tolerance) {}

    [[nodiscard]] inline std::string Name() const final { return "OccPos"; };
    [[nodiscard]] inline std::string Param1() const final { return std::to_string(tolerance); };

    [[nodiscard]] bool ToleranceMatch(const Series &series, uint offset, const Series &window) const {
        const double tempTolerance = tolerance + series[offset];

        for (uint i = 1; i < window.size(); i++)
            if (std::abs(series[i + offset] - window.at(i)) > tempTolerance)
                return false;

        return true;
    }

    [[nodiscard]] double GenerateValue(const Series &series, const Series &window) const final {
        const uint maxI = series.size() - window.size();
        for (uint i = 0; i < maxI; ++i) {
            if (ToleranceMatch(series, i, window))
                return (double) i / (double) maxI;
        }

        return 1;
    }
private:
    const double tolerance;
};

#endif //FEATUREEXTRACTION_OCCPOS_H
