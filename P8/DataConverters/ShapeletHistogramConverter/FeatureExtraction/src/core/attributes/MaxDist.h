#ifndef FEATUREEXTRACTION_MAXDIST_H
#define FEATUREEXTRACTION_MAXDIST_H

#include <string>
#include "Attribute.h"

class MaxDist : public Attribute {
public:
    [[nodiscard]] inline std::string Name() const final { return "MaxDist"; };

    [[nodiscard]] static double MaximumDistance(const Series &series, uint offset, const Series &window) {
        const double yOffset = series[offset];
        double dist = 0;

        for (uint i = 1; i < window.size(); i++)
            dist += std::abs(series[i + offset] - (yOffset + window.at(i)));

        return dist;
    }

    [[nodiscard]] double GenerateValue(const Series &series, const Series &window) const final {
        uint currentMaxDist = 0;
        const double maxDist = (uint) window.size();

        for (uint i = 0; i < series.size() - window.size(); ++i) {
            const double dist = MaximumDistance(series, i, window);
            if (dist > currentMaxDist)
                currentMaxDist = dist;
        }

        return currentMaxDist / maxDist;
    }
};

#endif //FEATUREEXTRACTION_MAXDIST_H
