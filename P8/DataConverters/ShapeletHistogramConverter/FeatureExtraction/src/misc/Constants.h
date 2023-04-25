#ifndef FEATUREEXTRACTION_CONSTANTS_H
#define FEATUREEXTRACTION_CONSTANTS_H

#include <utility>
#include <vector>
#include <array>
#include <random>

#define DOUBLE_MAX 99999999

namespace {
    std::random_device rd;
    std::mt19937 g(rd());
}

// Already defined in Linux, but redefined here for compatibility
using uint = unsigned int;

constexpr uint MAX_CLASSES = 64;
constexpr uint MAX_THREADS = 1;

#endif //FEATUREEXTRACTION_CONSTANTS_H
