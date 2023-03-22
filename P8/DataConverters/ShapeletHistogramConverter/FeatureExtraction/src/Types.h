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

std::string AttributeTypeToString(AttributeType type) {
    switch (type) {
        case AttributeType::None:
            return "None";
        case AttributeType::Frequency:
            return "Freq";
        case AttributeType::MinDist:
            return "MinDist";
    }
}

std::string AttributeTypeDesc(AttributeType type) {
    switch (type) {
        case AttributeType::None:
            return "No attribute. Likely a bug.";
        case AttributeType::Frequency:
            return "How frequently a shapelet occurs in a given time series.";
        case AttributeType::MinDist:
            return "The minimum distance between a shapelet and a given time series.";
    }
}

std::string Param1Desc(AttributeType type) {
    switch (type) {
        case AttributeType::None:
            return "Not used.";
        case AttributeType::Frequency:
            return "Tolerance. How far away a point can be from the shapelet, while still being considered a match.";
        case AttributeType::MinDist:
            return "Not used.";
    }
}

struct Attribute {
    AttributeType type;
    std::optional<double> param1;
    explicit Attribute(AttributeType type) : type(type) {};
    Attribute(AttributeType type, double param1) : type(type), param1(param1) {};
};

const std::vector<Attribute> Attributes {
    Attribute(AttributeType::Frequency, 0.01),
    Attribute(AttributeType::Frequency, 0.02),
    Attribute(AttributeType::Frequency, 0.04),
    Attribute(AttributeType::Frequency, 0.08),
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
    const double gain;
    const Attribute attribute;
    const std::vector<double> shapelet;

    Feature(const std::vector<double> &shapelet, const Attribute attribute, double gain)
            : shapelet(shapelet), attribute(attribute), gain(gain) {}
};

struct Split {
    const std::vector<LabelledSeries> below;
    const std::vector<LabelledSeries> above;
    Split(const std::vector<LabelledSeries> &below, const std::vector<LabelledSeries> &above) : below(below), above(above) {}
};

#endif //FEATUREEXTRACTION_TYPES_H
