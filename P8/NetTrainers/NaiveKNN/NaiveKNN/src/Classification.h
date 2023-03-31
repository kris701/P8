#ifndef CLASSIFIER_CLASSIFICATION_H
#define CLASSIFIER_CLASSIFICATION_H

#include <set>
#include <valarray>
#include "Types.h"

namespace Classification {
    namespace KNN {
        static std::multiset<std::pair<double, int>> GenerateNeighbours(const MappedSeries &trainData,
                                                                        const Series &series) {
            std::multiset<std::pair<double, int>> neighbours;
            for (const auto &trainSet : trainData) {
                for (const auto &trainFeature : trainSet.second) {
                    double tempDist = 0;
                    for (uint i = 0; i < trainFeature.size(); i++)
                        tempDist += std::pow(trainFeature.at(i) - series.at(i), 2);
                    const double dist = std::sqrt(tempDist);
                    neighbours.emplace(dist, trainSet.first);
                }
            }
            return neighbours;
        }

        static uint Choose(const std::multiset<std::pair<double, int>> &neighbours,
                          uint k) {
            if (neighbours.empty())
                throw std::logic_error("Must have atleast a single neighbour");

            ClassCount count{ 0 };
            uint bestIndex = 0;

            uint i = 0;
            for (const auto &neighbours : neighbours) {
                count[neighbours.second]++;
                if (count[neighbours.second] > count[bestIndex])
                    bestIndex = neighbours.second;
                if (++i >= k)
                    break;
            }

            return bestIndex;
        }

        static uint Classify(const MappedSeries &trainData, const Series &series, uint k) {
            const auto neighbours = GenerateNeighbours(trainData, series);
            return Choose(neighbours, k);
        }
    }
}

#endif //CLASSIFIER_CLASSIFICATION_H
