#ifndef CLASSIFIER_TYPES_H
#define CLASSIFIER_TYPES_H

#include <vector>
#include <unordered_map>
#include <array>

using uint = unsigned int;
constexpr uint MAX_CLASSES = 512;
using ClassCount = std::array<uint, MAX_CLASSES>;

using Series = std::vector<double>;
using SeriesSet = std::vector<Series>;
using MappedSeries = std::unordered_map<uint, SeriesSet>;

#endif //CLASSIFIER_TYPES_H
