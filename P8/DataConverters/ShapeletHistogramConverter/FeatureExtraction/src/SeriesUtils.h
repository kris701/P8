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

    [[nodiscard]] static std::vector<LabelledSeries> Combine(const std::vector<LabelledSeries> &seriesA, const std::vector<LabelledSeries> &seriesB) {
        std::vector<LabelledSeries> combined;

        for (const auto &seriesSet : { seriesA, seriesB })
            for (const auto &series : seriesSet)
                combined.push_back(series);

        return combined;
    }

    [[nodiscard]] static std::pair<std::vector<LabelledSeries>, std::vector<LabelledSeries>>
    Split (const std::vector<LabelledSeries>& series, uint firstSize) {
        std::vector<LabelledSeries> first;
        std::vector<LabelledSeries> second;

        for (uint i = 0; i < firstSize; i++)
            first.push_back(series.at(i));
        for (uint i = firstSize; i < series.size(); i++)
            second.push_back(series.at(i));

        return { first, second };
    }

    [[nodiscard]] static std::pair<std::vector<LabelledSeries>, std::vector<LabelledSeries>> Split
            (const std::vector<LabelledSeries>& series, double split) {
        if (std::ceil(split) == std::floor(split)) // has no decimal point
            return Split(series, (uint) std::round(split));
        else
            return Split(series, (uint) (split * (double) series.size()));
    }

    [[nodiscard]] static std::unordered_map<int, std::vector<Series>> ToMap
    (const std::vector<LabelledSeries> &series) {
        std::unordered_map<int, std::vector<Series>> map;

        for (const auto &s : series)
            map[s.label].push_back(s.series);

        return map;
    }
}

#endif //FEATUREEXTRACTION_SERIESUTILS_H
