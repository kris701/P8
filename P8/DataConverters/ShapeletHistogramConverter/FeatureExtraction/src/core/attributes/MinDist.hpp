#ifndef FEATUREEXTRACTION_MINDIST_H
#define FEATUREEXTRACTION_MINDIST_H

#include <numeric>
#include <string>
#include "Attribute.hpp"
#include "types/Series.hpp"

class MinDist : public Attribute {
public:
    [[nodiscard]] inline std::string Name() const final { return "MinDist"; };

    [[nodiscard]] static double MinimumDistance(const Series &series, uint offset, const Series &window, double currentMin) {
        const double yOffset = series[offset];
        double dist = 0;

        for (uint i = 1; i < window.size(); i++) {
            dist += std::abs(series[i + offset] - (yOffset + window.at(i)));
            if (dist > currentMin)
                return dist;
        }

        return dist;
    }

    [[nodiscard]] double GenerateValue(const Series &series, const Series &window) const final {
        const double maxDist = (uint) window.size();
        double minDist = maxDist;

        for (uint i = 0; i <= series.size() - window.size(); ++i) {
            const double dist = MinimumDistance(series, i, window, minDist);
            if (dist < minDist)
                minDist = dist;
        }

        return minDist / maxDist;
    }
};

#endif //FEATUREEXTRACTION_MINDIST_H
