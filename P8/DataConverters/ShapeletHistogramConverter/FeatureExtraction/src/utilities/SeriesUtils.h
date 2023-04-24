#ifndef FEATUREEXTRACTION_SERIESUTILS_H
#define FEATUREEXTRACTION_SERIESUTILS_H

#include <unordered_set>
#include <unordered_map>
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

    [[nodiscard]] static std::unordered_map<uint, std::vector<Series>> ToMap
    (const std::vector<LabelledSeries> &series) {
        std::unordered_map<uint, std::vector<Series>> map;

        for (const auto &s : series)
            map[s.label].push_back(s.series);

        return map;
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
