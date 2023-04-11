#ifndef FEATUREEXTRACTION_CONSTANTS_H
#define FEATUREEXTRACTION_CONSTANTS_H

#include <utility>
#include <vector>
#include <array>
#include <random>

#define DOUBLE_MAX 99999999

std::random_device rd;
std::mt19937 g(rd());

using uint = unsigned int;
constexpr uint MAX_CLASSES = 64;
constexpr uint maxThreads = 100;

using Series = std::vector<double>;
using ClassCount = std::array<uint, MAX_CLASSES>;

struct LabelledSeries {
    int label;
    Series series;
    LabelledSeries(int label, const Series &series) : label(label), series(series) {};
};



#endif //FEATUREEXTRACTION_CONSTANTS_H
