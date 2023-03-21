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

    [[nodiscard]] static std::unordered_map<int, std::vector<Series>> ToMap
    (const std::vector<LabelledSeries> &series) {
        std::unordered_map<int, std::vector<Series>> map;

        for (const auto &s : series)
            map[s.label].push_back(s.series);

        return map;
    }

    [[nodiscard]] static std::pair<std::vector<LabelledSeries>, std::vector<LabelledSeries>> Split
            (const std::vector<LabelledSeries>& series, double split) {
        std::vector<LabelledSeries> first;
        std::vector<LabelledSeries> second;

        if (split == 1) {
            for (const auto &serie : series)
                second.emplace_back(serie);
            return { first, second };
        }

        const auto mapped = ToMap(series);
        for (const auto &seriesSet : mapped) {
            const uint count = seriesSet.second.size();
            // If no decimal point round split to count, else times split with number of series of given class
            const uint firstSize = (std::round(split) == split) ? (uint) std::round(split) : (uint) (split * (double) count);
            for (uint i = 0; i < firstSize; i++)
                first.emplace_back(seriesSet.first, seriesSet.second.at(i));
            for (uint i = firstSize; i < count; i++)
                second.emplace_back(seriesSet.first, seriesSet.second.at(i));
        }

        return { first, second };
    }
}

#endif //FEATUREEXTRACTION_SERIESUTILS_H
