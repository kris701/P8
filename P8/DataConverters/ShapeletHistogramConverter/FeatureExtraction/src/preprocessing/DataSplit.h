#ifndef FEATUREEXTRACTION_DATASPLIT_H
#define FEATUREEXTRACTION_DATASPLIT_H

#include <unordered_map>
#include <stdexcept>
#include "misc/Constants.h"

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
            std::unordered_set<uint> chosenIndexSet;
            for (const auto &index : chosenIndexes)
                chosenIndexSet.emplace(index);

            for (const auto &index : indexes)
                if (chosenIndexSet.contains(index))
                    train[dataSet.first].push_back(dataSet.second.at(index));
                else
                    test[dataSet.first].push_back(dataSet.second.at(index));
        }

        return { train, test };
    }
};

#endif //FEATUREEXTRACTION_DATASPLIT_H
