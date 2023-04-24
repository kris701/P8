#ifndef FEATUREEXTRACTION_CLASSCOUNTUTILS_H
#define FEATUREEXTRACTION_CLASSCOUNTUTILS_H

#include "misc/Constants.h"
#include "types/ClassCount.h"

namespace ClassCountUtils {
    /// @brief Takes in a map of ClassCount's and splits it into two based on if the key is lower than the given splitPoint value
    /// @param values A map of ClassCount's
    /// @param splitPoint What value the ClassCount's should be split by
    /// @return A pair of class counts
    [[nodiscard]] static std::pair<ClassCount, ClassCount> GetSplit(const std::map<double, ClassCount>& values, double splitPoint) {
        ClassCount lowerCount;
        ClassCount upperCount;
        for (const auto& value : values)
            if (value.first < splitPoint)
                for (int i = 0; i < MAX_CLASSES; i++)
                    lowerCount[i] += value.second[i];
            else
                for (int i = 0; i < MAX_CLASSES; i++)
                    upperCount[i] += value.second[i];
        return std::make_pair(lowerCount, upperCount);
    }
}

#endif
