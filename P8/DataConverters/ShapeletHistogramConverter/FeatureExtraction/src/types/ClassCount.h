#ifndef FEATUREEXTRACTION_CLASSCOUNT_H
#define FEATUREEXTRACTION_CLASSCOUNT_H

#include <numeric>
#include "misc/Constants.h"

struct ClassCount : public std::array<uint, MAX_CLASSES> {
    /// @return The sum of counts
    [[nodiscard]] inline uint Total() const { return std::accumulate(this->begin(), this->end(), (uint) 0); }
};


#endif //FEATUREEXTRACTION_CLASSCOUNT_H
