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

    static std::vector<std::vector<std::string>> ClassCountCSV(const ClassCount &cc1,
                                                               const ClassCount &cc2) {
        std::vector<std::vector<std::string>> values;

        for (uint i = 0; i < MAX_CLASSES; i++)
            if (cc1[i] > 0 || cc2[i] > 0)
                values.push_back({std::to_string(i), std::to_string(cc1[i]), std::to_string(cc2[i])});

        return values;
    }

    static std::vector<std::vector<std::vector<std::string>>> ClassCountCSV(const std::vector<LabelledSeries> &trainData,
                                                               const std::vector<Feature> &features) {
        std::vector<std::vector<std::vector<std::string>>> files;
        for (const auto &feature : features) {
            const double splitPoint = InformationGain::GetOptimalSplitPoint(feature.attribute->GenerateValues(trainData, feature.shapelet));
            const auto split = feature.attribute->SplitSeries(trainData, feature.shapelet, splitPoint);
            const auto cc1 = SeriesUtils::GetCount(split[0]);
            const auto cc2 = SeriesUtils::GetCount(split[1]);
            files.push_back(ClassCountCSV(cc1, cc2));
        }
        return files;
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
