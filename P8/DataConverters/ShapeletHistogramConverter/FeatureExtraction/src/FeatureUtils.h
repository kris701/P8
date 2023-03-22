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
}

#endif //FEATUREEXTRACTION_FEATUREUTILS_H
