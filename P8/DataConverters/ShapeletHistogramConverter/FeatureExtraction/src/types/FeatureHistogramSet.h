#ifndef FEATUREEXTRACTION_FEATUREHISTOGRAMSET_H
#define FEATUREEXTRACTION_FEATUREHISTOGRAMSET_H

#include <vector>
#include "FeatureHistogram.h"

struct FeatureHistogramSet {
    explicit FeatureHistogramSet(uint expectedSize) { histograms.reserve(expectedSize); }
    void Add(uint label, const FeatureHistogram &histogram) { histograms[label].push_back(histogram); }
    operator std::unordered_map<uint, std::vector<std::vector<double>>>() const {
        std::unordered_map<uint, std::vector<std::vector<double>>> values;

        for (const auto &vSet : histograms)
            for (const auto &v : vSet.second)
                values[vSet.first].push_back(v);

        return values;
    }
private:
    std::unordered_map<uint, std::vector<FeatureHistogram>> histograms;
};

#endif //FEATUREEXTRACTION_FEATUREHISTOGRAMSET_H
