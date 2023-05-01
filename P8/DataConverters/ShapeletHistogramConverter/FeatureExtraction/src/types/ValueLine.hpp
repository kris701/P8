#ifndef FEATUREEXTRACTION_VALUELINE_H
#define FEATUREEXTRACTION_VALUELINE_H

#include <map>
#include "ClassCount.hpp"

struct ValueLine : public std::map<double, ClassCount> {
    ValueLine() = default;
    ValueLine(const std::map<double, ClassCount> &values) : std::map<double, ClassCount>(values) {}
    /// @brief Takes in a map of ClassCount's and splits it into two based on if the key is lower than the given splitPoint value
    /// @param splitPoint What value the ClassCount's should be split by
    /// @return A pair of class counts
    [[nodiscard]] std::pair<ClassCount, ClassCount> Split(double splitPoint) const {
        ClassCount lowerCount{0};
        ClassCount upperCount{0};
        for (const auto& value : *this)
            if (value.first < splitPoint)
                for (int i = 0; i < MAX_CLASSES; i++)
                    lowerCount[i] += value.second[i];
            else
                for (int i = 0; i < MAX_CLASSES; i++)
                    upperCount[i] += value.second[i];
        return std::make_pair(lowerCount, upperCount);
    }
};

#endif //FEATUREEXTRACTION_VALUELINE_H
