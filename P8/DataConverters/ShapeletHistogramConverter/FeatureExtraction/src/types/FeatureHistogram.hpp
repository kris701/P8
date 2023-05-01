#ifndef FEATUREEXTRACTION_FEATUREHISTOGRAM_H
#define FEATUREEXTRACTION_FEATUREHISTOGRAM_H

#include "../misc/Constants.hpp"
#include "FeatureSet.hpp"

struct FeatureHistogram : public std::vector<double> {
    explicit FeatureHistogram(const std::vector<double> &values) : std::vector<double>(values) {}
};

#endif //FEATUREEXTRACTION_FEATUREHISTOGRAM_H
