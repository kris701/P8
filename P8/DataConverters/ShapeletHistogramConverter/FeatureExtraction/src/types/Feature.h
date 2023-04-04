#ifndef FEATUREEXTRACTION_FEATURE_H
#define FEATUREEXTRACTION_FEATURE_H

#include "core/attributes//Attribute.h"
#include "misc/Constants.h"

struct Feature {
    const double gain;
    const std::shared_ptr<Attribute> attribute;
    const std::vector<double> shapelet;

    Feature(const std::vector<double> &shapelet, const std::shared_ptr<Attribute> attribute, double gain)
            : shapelet(shapelet), attribute(attribute), gain(gain) {}
};

#endif //FEATUREEXTRACTION_FEATURE_H
