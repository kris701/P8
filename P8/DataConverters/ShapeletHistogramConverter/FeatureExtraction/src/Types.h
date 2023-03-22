#ifndef FEATUREEXTRACTION_TYPES_H
#define FEATUREEXTRACTION_TYPES_H

#include <utility>
#include <vector>
#include <array>

using uint = unsigned int;
constexpr uint MAX_CLASSES = 20;

using Series = std::vector<double>;
using ClassCount = std::array<uint, MAX_CLASSES>;

enum class AttributeType {
    None,
    Frequency,
    MinDist
};

struct Attribute {
    AttributeType type;
    std::optional<double> param1;
    explicit Attribute(AttributeType type) : type(type) {};
    Attribute(AttributeType type, double param1) : type(type), param1(param1) {};
};

const std::vector<Attribute> Attributes {
    Attribute(AttributeType::Frequency, 0.01),
    Attribute(AttributeType::Frequency, 0.1),
    Attribute(AttributeType::Frequency, 0.2),
    Attribute(AttributeType::Frequency, 0.4),
    Attribute(AttributeType::Frequency, 0.8),
    Attribute(AttributeType::MinDist),
};

struct LabelledSeries {
    int label;
    Series series;
    LabelledSeries(int label, const Series &series) : label(label), series(series) {};
};

struct Feature {
    const Attribute attribute;
    const std::vector<double> shapelet;
    const double gain;

    Feature(const std::vector<double> &shapelet, const Attribute attribute, double gain)
            : shapelet(shapelet), attribute(attribute), gain(gain) {}
};

struct Split {
    const std::vector<LabelledSeries> below;
    const std::vector<LabelledSeries> above;
    Split(const std::vector<LabelledSeries> &below, const std::vector<LabelledSeries> &above) : below(below), above(above) {}
};

#endif //FEATUREEXTRACTION_TYPES_H
