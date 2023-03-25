#ifndef FEATUREEXTRACTION_CLASSIFICATION_H
#define FEATUREEXTRACTION_CLASSIFICATION_H

#include <unordered_map>
#include <set>
#include "Types.h"
#include "Feature.h"

namespace Classification {
    std::multiset<std::pair<double, int>> GenerateNeighbours(const std::unordered_map<int, std::vector<Series>> &trainFeatures,
                                                             const std::vector<double> &values) {
        std::multiset<std::pair<double, int>> neighbours;
        for (const auto &trainFeatureSet : trainFeatures) {
            for (const auto &trainFeature : trainFeatureSet.second) {
                double tempDist = 0;
                for (uint i = 0; i < trainFeature.size(); i++)
                    tempDist += std::pow(trainFeature.at(i) - values.at(i), 2);
                const double dist = std::sqrt(tempDist);
                neighbours.emplace(dist, trainFeatureSet.first);
            }
        }
        return neighbours;
    }

    int KNearestNeighbours(const std::vector<Feature> &features, const std::unordered_map<int, std::vector<Series>> &trainFeatures,
                           const Series &series, uint k) {
        std::vector<double> values;
        for (const auto &feature : features)
            values.push_back(feature.attribute->GenerateValue(series, feature.shapelet));

        const auto neighbours = GenerateNeighbours(trainFeatures, values);

        ClassCount count { 0 };
        uint i = 0;
        for (const auto &neighbour : neighbours) {
            count[neighbour.second]++;
            if (++i >= k)
                break;
        }

        std::optional<int> bestGuess;
        uint bestGuessCount;
        for (uint t = 0; t < MAX_CLASSES; t++)
            if (!bestGuess.has_value() || count[t] > bestGuessCount) {
                bestGuess = t;
                bestGuessCount = count[t];
            }

        return bestGuess.value();
    }
}

#endif //FEATUREEXTRACTION_CLASSIFICATION_H
