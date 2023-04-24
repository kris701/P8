#ifndef FEATUREEXTRACTION_FEATUREHISTOGRAMBUILDER_H
#define FEATUREEXTRACTION_FEATUREHISTOGRAMBUILDER_H

#include <unordered_map>
#include "FeatureHistogram.h"
#include "FeatureSet.h"
#include "FeatureHistogramSet.h"

class FeatureHistogramBuilder {
public:
    static inline FeatureHistogram Build(const Series &series, const FeatureSet &features) {
        return FeatureHistogram(features.GenerateValues(series));
    }

    static inline FeatureHistogramSet BuildSet(const std::vector<LabelledSeries> &series, const FeatureSet &features) {
        auto histograms = FeatureHistogramSet();

        for (const auto &s : series)
            histograms[s.label].push_back(Build(s.series, features));

        return histograms;
    }
};

#endif //FEATUREEXTRACTION_FEATUREHISTOGRAMBUILDER_H
