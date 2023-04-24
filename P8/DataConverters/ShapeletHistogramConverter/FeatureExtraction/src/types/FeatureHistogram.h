#ifndef FEATUREEXTRACTION_FEATUREHISTOGRAM_H
#define FEATUREEXTRACTION_FEATUREHISTOGRAM_H

#include "../misc/Constants.h"
#include "FeatureSet.h"

struct FeatureHistogram {
    explicit FeatureHistogram(const std::vector<double> &values) : values(values) {
        for (const auto &value : this->values)
            assert(value >= 0 && value <= 1);
    }
    operator std::vector<double>() const { return values; }

private:
    std::vector<double> values;
};


#endif //FEATUREEXTRACTION_FEATUREHISTOGRAM_H
