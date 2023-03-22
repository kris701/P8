#ifndef FEATUREEXTRACTION_TYPES_H
#define FEATUREEXTRACTION_TYPES_H

#include <utility>
#include <vector>
#include <array>

using uint = unsigned int;
constexpr uint MAX_CLASSES = 20;

using Series = std::vector<double>;
using ClassCount = std::array<uint, MAX_CLASSES>;

struct LabelledSeries {
    int label;
    Series series;
    LabelledSeries(int label, const Series &series) : label(label), series(series) {};
};

#endif //FEATUREEXTRACTION_TYPES_H
