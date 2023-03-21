#ifndef FEATUREEXTRACTION_SERIESUTILS_H
#define FEATUREEXTRACTION_SERIESUTILS_H

#include "Types.h"

namespace SeriesUtils {
    [[nodiscard]] static ClassCount GetCount(const std::vector<LabelledSeries> &series) {
        ClassCount counts { 0 };
        for (const auto &s : series)
            counts[s.label]++;
        return counts;
    }

    [[nodiscard]] static double MinValue(const std::vector<LabelledSeries> &series) {
        double minValue = 9999;
        for (const auto &s : series)
            for (const auto &v : s.series)
                minValue = std::min(v, minValue);
        return minValue;
    }

    [[nodiscard]] static double MaxValue(const std::vector<LabelledSeries> &series) {
        double minValue = -9999;
        for (const auto &s : series)
            for (const auto &v : s.series)
                minValue = std::max(v, minValue);
        return minValue;
    }
}

#endif //FEATUREEXTRACTION_SERIESUTILS_H
