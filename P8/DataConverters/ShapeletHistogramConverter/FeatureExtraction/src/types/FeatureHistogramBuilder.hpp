#ifndef FEATUREEXTRACTION_FEATUREHISTOGRAMBUILDER_H
#define FEATUREEXTRACTION_FEATUREHISTOGRAMBUILDER_H

#include <unordered_map>
#include "FeatureHistogram.hpp"
#include "FeatureSet.hpp"
#include "FeatureHistogramSet.hpp"

class FeatureHistogramBuilder {
public:
    static inline FeatureHistogram Build(const Series &series, const FeatureSet &features) {
        return FeatureHistogram(features.GenerateHistogram(series));
    }

    static inline FeatureHistogramSet BuildSet(const SeriesMap &series, const FeatureSet &features) {
        auto histograms = FeatureHistogramSet();

        for (const auto &seriesSet : series)
            for (const auto &s : seriesSet.second)
                histograms[seriesSet.first].push_back(Build(s, features));

        return histograms;
    }
};

#endif //FEATUREEXTRACTION_FEATUREHISTOGRAMBUILDER_H
