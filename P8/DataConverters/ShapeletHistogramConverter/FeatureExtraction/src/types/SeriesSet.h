#ifndef FEATUREEXTRACTION_SERIESSET_H
#define FEATUREEXTRACTION_SERIESSET_H

#include <vector>
#include "Series.h"

struct SeriesSet : public std::vector<Series> {
    inline operator std::vector<std::vector<double>>() const {
        std::vector<std::vector<double>> values;

        for (const auto &v : *this)
            values.push_back(v);

        return values;
    }
};


#endif //FEATUREEXTRACTION_SERIESSET_H
