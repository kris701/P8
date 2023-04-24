#ifndef FEATUREEXTRACTION_SERIESMAP_H
#define FEATUREEXTRACTION_SERIESMAP_H

#include <unordered_map>
#include "Series.h"
#include "misc/Constants.h"

struct SeriesMap : public std::unordered_map<uint, std::vector<Series>> {
    explicit SeriesMap(const std::vector<LabelledSeries> &series) {
        for (const auto &s : series)
            (*this)[s.label].push_back(s.series);
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
