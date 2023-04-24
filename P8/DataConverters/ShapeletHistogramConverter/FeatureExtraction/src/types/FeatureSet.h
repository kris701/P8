#ifndef FEATUREEXTRACTION_FEATURESET_H
#define FEATUREEXTRACTION_FEATURESET_H

#include <vector>
#include "Feature.h"

struct FeatureSet {
public:
    explicit FeatureSet(uint expectedCount) { features.reserve(expectedCount); }
    explicit FeatureSet(std::vector<Feature> &features) : features(std::move(features)) {}
    inline uint Size() { return features.size(); }
    inline void Add(Feature &feature) { features.push_back(std::move(feature)); }
    inline bool Contains(const Feature &feature) { return std::find(features.begin(), features.end(), feature) != features.end(); }
    std::vector<Series> Shapelets();
    std::vector<std::vector<std::string>> FeatureCSV(const std::vector<std::string> &shapeletPaths);
    std::vector<double> GenerateValues(const Series &series) const;

private:
    std::vector<Feature> features;
};

std::vector<Series> FeatureSet::Shapelets() {
    std::vector<Series> shapelets;

    for (const auto &feature : features)
        shapelets.push_back(feature.shapelet);

    return shapelets;
}

std::vector<std::vector<std::string>> FeatureSet::FeatureCSV(const std::vector<std::string> &shapeletPaths) {
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

std::vector<double> FeatureSet::GenerateValues(const Series &series) const {
    std::vector<double> values;

    for (const auto &feature : features)
        values.push_back(feature.attribute->GenerateValue(series, feature.shapelet));

    return values;
}

#endif //FEATUREEXTRACTION_FEATURESET_H
