#ifndef FEATUREEXTRACTION_SERIESMAP_H
#define FEATUREEXTRACTION_SERIESMAP_H

#include <unordered_map>
#include <unordered_set>
#include "Series.h"
#include "misc/Constants.h"
#include "ClassCount.h"
#include "SeriesSet.h"

struct SeriesMap : public std::unordered_map<uint, SeriesSet> {
    // Returns the minimum value found in any series contained in the map
    inline double Min() const {
        return std::min_element((*this).begin(), (*this).end())->second.Min();
    }

    // Returns the maximum value found in any series contained in the map
    inline double Max() const {
        return std::max_element((*this).begin(), (*this).end())->second.Max();
    }

    // Returns how many series of each class is contained in the map
    ClassCount Count() const {
        ClassCount count;

        for (const auto &seriesSet : *this)
            count.at(seriesSet.first) += seriesSet.second.size();

        return count;
    }

    // Simply inserts all series
    void InsertAll(const SeriesMap &seriesMap) {
        for (const auto &seriesSet : seriesMap) {
            const auto i = seriesSet.first;
            (*this)[i].insert((*this)[i].end(), seriesMap.at(i).begin(), seriesMap.at(i).end());
        }
    }

    // Applies a function all series contained in the map
    template<typename T>
    inline void Apply(T&& lambda) {
        for (auto &seriesSet : (*this))
            seriesSet.second.Apply(lambda);
    }

    // Moves all values into the range 0-1
    void MinMaxNormalize() {
        const auto min = Min();
        const auto max = Max() - min;

        Apply([min, max](auto &value) {
            value -= min;
            value = value / max;
        });
    }

    void ForcePositiveRange() {
        std::unordered_set<int> labels;
        for (const auto &dataPoint : *this)
            labels.emplace(dataPoint.first);

        std::vector<int> labelPos;
        for (const auto &label : labels)
            labelPos.push_back(label);
        std::sort(labelPos.begin(), labelPos.end());

        SeriesMap tempMap;
        for (int index : labelPos)
            tempMap[index].insert(tempMap[index].end(), (*this).at(index).begin(), (*this).at(index).end());

        (*this) = tempMap;
    }

    inline operator std::unordered_map<uint, std::vector<std::vector<double>>>() const {
    std::unordered_map<uint, std::vector<std::vector<double>>> values;

    for (const auto &vSet : *this)
        for (const auto &v : vSet.second)
            values[vSet.first].push_back(v);

    return values;
    }
};

#endif //FEATUREEXTRACTION_SERIESMAP_H
