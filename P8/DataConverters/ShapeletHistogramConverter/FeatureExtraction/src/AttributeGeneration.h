#ifndef FEATUREEXTRACTION_ATTRIBUTEGENERATION_H
#define FEATUREEXTRACTION_ATTRIBUTEGENERATION_H

#include <stdexcept>
#include <cassert>
#include <map>
#include "Types.h"

namespace AttributeGeneration {
    namespace Frequency {
        [[nodiscard]] static bool ToleranceMatch(const Series &series, uint offset, const Series &window, double tolerance) {
            const double tempTolerance = tolerance + series[offset];

            for (uint i = 1; i < window.size(); i++)
                if (std::abs(series[i + offset] - window.at(i)) > tempTolerance)
                    return false;

            return true;
        }

        [[nodiscard]] static double GenerateFrequency(const Series &series, const Series &window, double tolerance) {
            uint checked = 0;
            uint matchCount = 0;

            for (uint i = 0; i < series.size() - window.size(); ++i) {
                if (ToleranceMatch(series, i, window, tolerance))
                    ++matchCount;
                ++checked;
            }

            return (double) matchCount / checked;
        }
    }

    namespace Distance {
        [[nodiscard]] static double Distance(const Series &series, uint offset, const Series &window) {
            double tempDist = 0;
            const double yOffset = series[offset];

            for (uint i = 1; i < window.size(); i++)
                tempDist += series[i + offset] - window[i] - yOffset;

            return tempDist;
        }

        // Assumes input values between 0, 1
        [[nodiscard]] static double MinimumDistance(const Series &series, const Series &window) {
            assert(window.size() <= series.size());

            const auto maxDist = (double) (window.size() - 1);
            double minDist = maxDist;

            for (uint i = 0; i < series.size() - window.size(); ++i) {
                double tempDist = Distance(series, i, window);
                if (tempDist < minDist)
                    minDist = tempDist;
            }

            return (double) minDist / maxDist;
        }
    }

    [[nodiscard]] static inline double GenerateValue(const Series &series, const Series &window, Attribute attribute) {
        assert(window.size() <= series.size());
        switch (attribute.type) {
            case AttributeType::Frequency:
                return Frequency::GenerateFrequency(series, window, attribute.param1.value());
            case AttributeType::MinDist:
                return Distance::MinimumDistance(series, window);
            case AttributeType::None:
            default:
                throw std::logic_error("Missing attribute in value generation.");
        }
    }

    [[nodiscard]] static std::map<double, ClassCount> GenerateValues(const std::vector<LabelledSeries> &series, const Series &window, Attribute attribute) {
        std::map<double, ClassCount> values;

        for (const auto &s : series)
            values[GenerateValue(s.series, window, attribute)][s.label]++;

        return values;
    }

    [[nodiscard]] static Split SplitSeries(const std::vector<LabelledSeries> &series, Attribute attribute, const Series &shapelet, double splitPoint) {
        std::vector<LabelledSeries> splitSeries[2]; // 0 below, 1 above

        for (const auto &s : series)
            splitSeries[GenerateValue(s.series, shapelet, attribute) > splitPoint].push_back(s);

        return Split(splitSeries[0], splitSeries[1]);
    }
}

#endif //FEATUREEXTRACTION_ATTRIBUTEGENERATION_H
