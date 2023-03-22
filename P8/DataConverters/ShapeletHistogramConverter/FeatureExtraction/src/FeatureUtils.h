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
                AttributeTypeToString(feature.attribute.type),
                feature.attribute.param1.has_value() ? std::to_string(feature.attribute.param1.value()) : "",
                "",
                "",
                shapeletPaths.at(i)});
        }

        return lines;
    }

    [[nodiscard]] static std::vector<std::string> AttributeHeader() {
        return {
            "Attribute",
            "Desc",
            "Param1 Desc",
            "Param2 Desc",
            "Param3 Desc"
        };
    }

    [[nodiscard]] static std::vector<std::vector<std::string>> AttributeCSV(const std::vector<Feature> &features) {
        std::unordered_set<AttributeType> attributesUsed;
        for (const auto &feature : features)
            attributesUsed.emplace(feature.attribute.type);

        std::vector<std::vector<std::string>> lines;

        for (const auto &attribute : attributesUsed) {
            lines.push_back({
                AttributeTypeToString(attribute),
                AttributeTypeDesc(attribute),
                Param1Desc(attribute),
                "",
                ""
            });
        }

        return lines;
    }
}

#endif //FEATUREEXTRACTION_FEATUREUTILS_H
