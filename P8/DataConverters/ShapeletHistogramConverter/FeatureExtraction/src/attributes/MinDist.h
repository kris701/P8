#ifndef FEATUREEXTRACTION_MINDIST_H
#define FEATUREEXTRACTION_MINDIST_H

#include <string>
#include "Attribute.h"

class MinDist : public Attribute {
public:
    [[nodiscard]] inline std::string Name() const final { return "MinDist"; };

    [[nodiscard]] static double MinimumDistance(const Series &series, uint offset, const Series &window, std::optional<double> currentMin) {
        double dist = 0;
        for (uint i = 1; i < window.size(); i++) {
            dist += std::abs(series[i + offset] - window.at(i));
            if (currentMin.has_value() && dist > currentMin.value())
                return dist;
        }

        return dist;
    }

    [[nodiscard]] double GenerateValue(const Series &series, const Series &window) const final {
        std::optional<double> minDist;
        const double maxDist = (uint) window.size();

        for (uint i = 0; i < series.size() - window.size(); ++i) {
            const double dist = MinimumDistance(series, i, window, minDist);
            if (!minDist.has_value() || dist < minDist.value())
                minDist = dist;
        }

        return minDist.value() / maxDist;
    }
};

#endif //FEATUREEXTRACTION_MINDIST_H
