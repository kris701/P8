#ifndef FEATUREEXTRACTION_SERIESUTILS_H
#define FEATUREEXTRACTION_SERIESUTILS_H

#include <unordered_set>
#include "misc/Constants.h"

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

    static void MinMaxNormalize(std::vector<LabelledSeries> &series) {
        const auto min = MinValue(series);
        const auto max = MaxValue(series) - min;

        // First move all values into positive range
        // Does this moving all values such that minimum is in zero
        for (auto &s : series)
            for (auto &p : s.series)
                    p -= min;

        // Normalize
        for (auto &s : series)
            for (auto &p : s.series)
                p = p / max;
    }

    static void ForcePositiveRange(std::vector<LabelledSeries> &series) {
        std::unordered_set<int> labels;
        for (const auto &dataPoint : series)
            labels.emplace(dataPoint.label);
        std::vector<int> labelPos;
        for (const auto &label : labels)
            labelPos.push_back(label);
        std::sort(labelPos.begin(), labelPos.end());
        for (uint i = 0; i < labelPos.size(); i++)
            for (auto &dataPoint : series)
                if (dataPoint.label == labelPos[i])
                    dataPoint.label = i;
    }
}

#endif //FEATUREEXTRACTION_SERIESUTILS_H
