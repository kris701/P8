#ifndef FEATUREEXTRACTION_CLASSCOUNT_H
#define FEATUREEXTRACTION_CLASSCOUNT_H

#include "misc/Constants.h"

struct ClassCount {
    ClassCount() = default;

    ClassCount(const ClassCount &cc) : classCount(cc) {}
    /// @brief Gets the total amount of samples
    /// @param values ClassCount of samples
    /// @return The total amount of samples
    [[nodiscard]] inline uint GetTotal() const { return std::accumulate(classCount.begin(), classCount.end(), (uint) 0); }

    inline uint operator[](uint i) const { return classCount[i]; }
    inline uint& operator[](uint i) { return classCount[i]; }
    inline operator std::array<uint, MAX_CLASSES>() const { return classCount; }

private:
    std::array<uint, MAX_CLASSES> classCount{ 0 };
};


#endif //FEATUREEXTRACTION_CLASSCOUNT_H
