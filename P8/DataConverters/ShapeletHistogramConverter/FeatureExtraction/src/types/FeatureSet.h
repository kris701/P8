#ifndef FEATUREEXTRACTION_FEATURESET_H
#define FEATUREEXTRACTION_FEATURESET_H

#include <vector>
#include "Feature.h"
#include "SeriesSet.h"

struct FeatureSet : public std::vector<Feature> {
    inline bool Contains(const Feature &feature) { return std::find(this->begin(), this->end(), feature) != this->end(); }
    [[nodiscard]] SeriesSet Shapelets() const;
    [[nodiscard]] std::vector<std::vector<std::string>> FeatureCSV(const std::vector<std::string> &shapeletPaths) const;
    [[nodiscard]] std::vector<double> GenerateValues(const Series &series) const;
};

SeriesSet FeatureSet::Shapelets() const {
    SeriesSet shapelets;

    for (const auto &feature : *this)
        shapelets.push_back(feature.shapelet);

    return shapelets;
}

std::vector<std::vector<std::string>> FeatureSet::FeatureCSV(const std::vector<std::string> &shapeletPaths) const {
    std::vector<std::vector<std::string>> lines;

    for (uint i = 0; i < this->size(); i++) {
        const auto& feature = this->at(i);
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

    for (const auto &feature : *this)
        values.push_back(feature.attribute->GenerateValue(series, feature.shapelet));

    return values;
}

#endif //FEATUREEXTRACTION_FEATURESET_H
