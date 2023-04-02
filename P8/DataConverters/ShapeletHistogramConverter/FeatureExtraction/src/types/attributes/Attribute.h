#ifndef FEATUREEXTRACTION_ATTRIBUTE_H
#define FEATUREEXTRACTION_ATTRIBUTE_H

#include <cassert>
#include <map>
#include <optional>
#include "misc/Constants.h"

class Attribute {
public:
    [[nodiscard]] virtual inline std::string Name() const = 0;
    [[nodiscard]] virtual inline std::string Param1() const { return ""; };
    [[nodiscard]] virtual inline std::string Param2() const { return ""; };
    [[nodiscard]] virtual inline std::string Param3() const { return ""; };
    [[nodiscard]] virtual double GenerateValue(const Series &series, const Series &window) const = 0;

    [[nodiscard]] std::map<double, ClassCount> GenerateValues(const std::vector<LabelledSeries> &series, const Series &window) const {
        std::map<double, ClassCount> values;

        for (const auto &s : series)
            values[GenerateValue(s.series, window)][s.label]++;

        return values;
    }

    [[nodiscard]] std::array<std::vector<LabelledSeries>, 2> SplitSeries
            (const std::vector<LabelledSeries> &series, const Series &shapelet, double splitPoint) const {
        std::array<std::vector<LabelledSeries>, 2> splitSeries; // 0 below, 1 above

        for (const auto &s : series)
            splitSeries[GenerateValue(s.series, shapelet) > splitPoint].push_back(s);

        return splitSeries;
    }
};

#endif //FEATUREEXTRACTION_ATTRIBUTE_H
