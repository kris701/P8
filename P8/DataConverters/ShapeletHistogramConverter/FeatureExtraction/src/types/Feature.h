#ifndef FEATUREEXTRACTION_FEATURE_H
#define FEATUREEXTRACTION_FEATURE_H

#include "core/attributes//Attribute.h"
#include "misc/Constants.h"

struct Feature {
    const double gain;
    const std::shared_ptr<Attribute> attribute;
    const Series shapelet;

    Feature(const Feature &f) = default;
    Feature(const Series &shapelet, const std::shared_ptr<Attribute> attribute, double gain)
            : shapelet(shapelet), attribute(attribute), gain(gain) {}

    bool operator==(const auto &rhs) {
        return (
                attribute == rhs.attribute &&
                shapelet == rhs.shapelet
                );
    }

    // Returns header to be used in csv formats
    static std::vector<std::string> Header() {
        return {
                "Gain",
                "Attribute",
                "Param1",
                "Param2",
                "Param3",
                "Shapelet"
        };
    }
};

#endif //FEATUREEXTRACTION_FEATURE_H
