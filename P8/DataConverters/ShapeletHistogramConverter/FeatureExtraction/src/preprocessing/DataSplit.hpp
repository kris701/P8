#ifndef FEATUREEXTRACTION_DATASPLIT_H
#define FEATUREEXTRACTION_DATASPLIT_H

#include <unordered_map>
#include <stdexcept>
#include "../misc/Constants.hpp"
#include "../types/SeriesMap.hpp"

namespace DataSplit {
    struct Result {
        const SeriesMap train;
        const SeriesMap test;
        Result(SeriesMap train, SeriesMap test) :
                train(std::move(train)), test(std::move(test)){};
    };

    Result Split(const SeriesMap &dataPoints, uint split) {
        SeriesMap train;
        SeriesMap test;

        for (const auto &dataSet : dataPoints) {
            uint tempSplit = std::min(split, (uint) dataSet.second.size());
            std::vector<uint> indexes(dataSet.second.size());
            std::iota(indexes.begin(), indexes.end(), 0);

            std::vector<uint> chosenIndexes;
            std::sample(indexes.begin(), indexes.end(), std::back_inserter(chosenIndexes), tempSplit, g);
            // If fewer than split was chosen, chose some as duplicate
            if (split - (uint) chosenIndexes.size() > 0)
                std::sample(indexes.begin(), indexes.end(), std::back_inserter(chosenIndexes), split - (uint) chosenIndexes.size(), g);

            std::unordered_map<uint, uint> indexCount;
            for (const auto &index : chosenIndexes)
                ++indexCount[index];

            for (const auto &index : indexes)
                if (indexCount.contains(index))
                    for (uint i = 0; i < indexCount.at(index); i++)
                        train[dataSet.first].push_back(dataSet.second.at(index));
                else
                    test[dataSet.first].push_back(dataSet.second.at(index));
        }

        return { train, test };
    }
};

#endif //FEATUREEXTRACTION_DATASPLIT_H
