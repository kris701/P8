#ifndef FEATUREEXTRACTION_FEATUREUTILS_H
#define FEATUREEXTRACTION_FEATUREUTILS_H

#include "Types.h"

namespace FeatureUtils {
    [[nodiscard]] static std::vector<Series> RetrieveShapelets(const std::vector<Feature> &features) {
        std::vector<Series> shapelets;

        for (const auto &feature : features)
            shapelets.push_back(feature.shapelet);

        return shapelets;
    }

    [[nodiscard]] static std::vector<std::string> FeatureHeader() {
        return {
            "Gain",
            "Attribute",
            "Param1",
            "Param2",
            "Param3",
            "Shapelet"
        };
    }

    [[nodiscard]] static std::vector<std::vector<std::string>> FeatureCSV
    (const std::vector<Feature> &features, const std::vector<std::string> &shapeletPaths) {
        std::vector<std::vector<std::string>> lines;

        for (uint i = 0; i < features.size(); i++) {
            const auto& feature = features.at(i);
            lines.push_back({
                std::to_string(feature.gain),
                feature.attribute->Name(),
                feature.attribute->Param1(),
                feature.attribute->Param2(),
                feature.attribute->Param3(),
                shapeletPaths.at(i)});
        }

        return lines;
    }

    std::vector<double> GenerateFeatureSeries(const Series &series, const std::vector<Feature> &features) {
        std::vector<double> featureSeries;

        for (const auto &feature : features)
            featureSeries.push_back(feature.attribute->GenerateValue(series, feature.shapelet));

        return featureSeries;
    }

    std::unordered_map<int, std::vector<std::vector<double>>> GenerateFeatureSeries
            (const std::vector<LabelledSeries> &series, const std::vector<Feature> &features) {
        std::unordered_map<int, std::vector<std::vector<double>>> featureSeriesSet;

        for (const auto &s : series)
            featureSeriesSet[s.label].push_back(GenerateFeatureSeries(s.series, features));

        return featureSeriesSet;
    }
}

#endif //FEATUREEXTRACTION_FEATUREUTILS_H
