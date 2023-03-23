#ifndef FEATUREEXTRACTION_FEATURE_H
#define FEATUREEXTRACTION_FEATURE_H

#include "attributes/Attribute.h"
#include "Types.h"

struct Feature {
    const double gain;
    const Attribute *attribute;
    const std::vector<double> shapelet;

    Feature(const std::vector<double> &shapelet, const Attribute *attribute, double gain)
            : shapelet(shapelet), attribute(attribute), gain(gain) {}
};

#endif //FEATUREEXTRACTION_FEATURE_H
