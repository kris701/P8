#ifndef FEATUREEXTRACTION_FEATUREHISTOGRAMSET_H
#define FEATUREEXTRACTION_FEATUREHISTOGRAMSET_H

#include <vector>
#include "FeatureHistogram.h"

struct FeatureHistogramSet : public std::unordered_map<uint, std::vector<FeatureHistogram>> {
    inline operator std::unordered_map<uint, std::vector<std::vector<double>>>() const {
        std::unordered_map<uint, std::vector<std::vector<double>>> values;

        for (const auto &vSet : *this)
            for (const auto &v : vSet.second)
                values[vSet.first].push_back(v);

        return values;
    }
};

#endif //FEATUREEXTRACTION_FEATUREHISTOGRAMSET_H
