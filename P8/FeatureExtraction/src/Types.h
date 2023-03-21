#ifndef FEATUREEXTRACTION_TYPES_H
#define FEATUREEXTRACTION_TYPES_H

#include <utility>
#include <vector>
#include <array>

using uint = unsigned int;
constexpr uint MAX_CLASSES = 20;

using Series = std::vector<double>;
using ClassCount = std::array<uint, MAX_CLASSES>;

enum class Attribute {
    None,
    Frequency
};

constexpr uint ATTRIBUTE_COUNT = 1;
constexpr Attribute ATTRIBUTES[ATTRIBUTE_COUNT] { Attribute::Frequency };

struct LabelledSeries {
    int label;
    Series series;
    LabelledSeries(int label, const Series &series) : label(label), series(series) {};
};

struct Feature {
    const std::vector<double> shapelet;
    const Attribute attribute;

    Feature(const std::vector<double> &shapelet, const Attribute attribute)
            : shapelet(shapelet), attribute(attribute) {}
};

struct Split {
    const std::vector<LabelledSeries> below;
    const std::vector<LabelledSeries> above;
    Split(const std::vector<LabelledSeries> &below, const std::vector<LabelledSeries> &above) : below(below), above(above) {}
};

#endif //FEATUREEXTRACTION_TYPES_H
