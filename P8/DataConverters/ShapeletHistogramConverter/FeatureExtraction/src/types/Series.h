#ifndef FEATUREEXTRACTION_SERIES_H
#define FEATUREEXTRACTION_SERIES_H

#include <vector>
#include <algorithm>

struct Series : public std::vector<double> {
    // Returns the minimum value found in the series
    [[nodiscard]] inline double Min() const { return *std::min_element((*this).begin(), (*this).end()); }
    [[nodiscard]] inline double Max() const { return *std::max_element((*this).begin(), (*this).end()); }
    // Applies a function all values
    template<typename T>
    inline void Apply(T&& lambda) {
        std::for_each((*this).begin(), (*this).end(), lambda);
    }
};

#endif //FEATUREEXTRACTION_SERIES_H
