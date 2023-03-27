#ifndef FEATUREEXTRACTION_FEATURE_H
#define FEATUREEXTRACTION_FEATURE_H

#include "attributes/Attribute.h"
#include "Types.h"

struct Feature {
    const double gain;
    const Attribute *attribute;
    const std::vector<double> shapelet;
    const ClassCount classes;

    Feature(const std::vector<double> &shapelet, const Attribute *attribute, double gain, const ClassCount &classes)
            : shapelet(shapelet), attribute(attribute), gain(gain), classes(classes) {}
};

#endif //FEATUREEXTRACTION_FEATURE_H
