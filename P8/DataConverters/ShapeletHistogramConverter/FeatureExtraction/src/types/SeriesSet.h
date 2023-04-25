#ifndef FEATUREEXTRACTION_SERIESSET_H
#define FEATUREEXTRACTION_SERIESSET_H

#include <vector>
#include "Series.h"

struct SeriesSet : public std::vector<Series> {
    [[nodiscard]] inline double Min() const {
        return std::min_element((*this).begin(), (*this).end())->Min();
    }

    [[nodiscard]] inline double Max() const {
        return std::max_element((*this).begin(), (*this).end())->Max();
    }

    // Applies a function all series contained in the set
    template<typename T>
    inline void Apply(T&& lambda) {
        for (auto &series : *this)
            series.Apply(lambda);
    }

    inline operator std::vector<std::vector<double>>() const {
        std::vector<std::vector<double>> values;

        for (const auto &v : *this)
            values.push_back(v);

        return values;
    }
};

#endif //FEATUREEXTRACTION_SERIESSET_H
