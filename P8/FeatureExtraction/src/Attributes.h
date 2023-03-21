#ifndef FEATUREEXTRACTION_ATTRIBUTES_H
#define FEATUREEXTRACTION_ATTRIBUTES_H

#include <stdexcept>
#include <cassert>
#include <map>
#include "Types.h"

namespace Attributes {
    namespace Frequency {
        [[nodiscard]] static bool ToleranceMatch(const Series &series, uint offset, const Series &window) {
            const double tempTolerance = 0.5 + series[offset];

            for (uint i = 1; i < window.size(); i++)
                if (std::abs(series[i + offset] - window.at(i)) > tempTolerance)
                    return false;

            return true;
        }

        [[nodiscard]] static double GenerateFrequency(const Series &series, const Series &window) {
            uint checked = 0;
            uint matchCount = 0;

            for (uint i = 0; i < series.size() - window.size(); ++i) {
                if (ToleranceMatch(series, i, window))
                    ++matchCount;
                ++checked;
            }

            return (double) matchCount / checked;
        }
    }

    [[nodiscard]] static inline double GenerateValue(const Series &series, const Series &window, Attribute attribute) {
        assert(window.size() <= series.size());
        switch (attribute) {
            case Attribute::Frequency:
                return Frequency::GenerateFrequency(series, window);
            case Attribute::None:
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

#endif //FEATUREEXTRACTION_ATTRIBUTES_H
