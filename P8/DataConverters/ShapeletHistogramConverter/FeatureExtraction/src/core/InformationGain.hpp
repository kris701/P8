#ifndef FEATUREEXTRACTION_INFORMATIONGAIN_H
#define FEATUREEXTRACTION_INFORMATIONGAIN_H

#include <cmath>
#include <map>
#include <numeric>
#include <exception>
#include <stdexcept>
#include "misc/Constants.hpp"
#include "types/ValueLine.hpp"

namespace InformationGain {
    /// @brief Calculates the entropy between a set of classes. Based on Definition 6 in http://alumni.cs.ucr.edu/~lexiangy/Shapelet/Shapelet.pdf
    /// @param total The total amount of value instances to compare against
    /// @param values The values to calculate entropy for
    /// @return A double, representing the entropy value
    [[nodiscard]] static double CalculateEntropy(uint total, const ClassCount &values) {
        if (total == 0)
            throw std::logic_error("Cannot calculate entropy with zero values!");

        double entropy = 0;
        for (int i = 0; i < MAX_CLASSES; i++) {
            if (values[i] > 0) {
                const double proportion = (double)values[i] / total;
                entropy += (-proportion) * (std::log2(proportion));
            }
        }
        return entropy;
    }

    [[nodiscard]] static inline double CalculateEntropy(const ClassCount &counts) {
        return CalculateEntropy(counts.Total(), counts);
    }

    /// @brief Gets the sum of entropy values within a split of values
    /// @param values The values to calculate entropy for
    /// @param splitPoint The split point to split the values parameter on
    /// @return A double, representing the entropy value
    [[nodiscard]] static double CalculateSplitEntropy(const ValueLine &values, double splitPoint) {
        const auto split = values.Split(splitPoint);
        const uint lowerTotal = split.first.Total();
        const uint upperTotal = split.second.Total();

        const uint total = lowerTotal + upperTotal;
        const double lowerProb = (double)lowerTotal / total;
        const double upperProb = (double)upperTotal / total;

        const double lowerEntropy = CalculateEntropy(total, split.first);
        const double upperEntropy = CalculateEntropy(total, split.second);

        return lowerEntropy * lowerProb + upperEntropy * upperProb;
    }

    /// @brief Finds the most optimal split point, based on entropy calculations
    /// @param values The values to find the split point for
    /// @return A double, representing the optimal splitting point in the given map
    [[nodiscard]] static double GetOptimalSplitPoint(const ValueLine &values) {
        if (values.size() < 2)
            throw std::logic_error("Trying to split a single point");
        double bestPoint = -1;
        double bestEntropy = DOUBLE_MAX;

        for (auto iter = values.begin(); std::next(iter, 1) != values.end(); iter++) {
            const double value = iter->first;
            const double nextValue = std::next(iter, 1)->first;
            const double splitPoint = value + (nextValue - value) / 2;
            const double splitEntropy = CalculateSplitEntropy(values, splitPoint);

            if (splitEntropy < bestEntropy) {
                bestPoint = splitPoint;
                bestEntropy = splitEntropy;
            }
        }

        if (bestPoint == -1)
            throw std::logic_error("No valid best point was found!");

        return bestPoint;
    }

    /// @brief Calculates the information gain for a map of ClassCount values.
    /// @param values The values to calcuate the information gain on
    /// @param priorEntropy An optional offset, so not to give back gains thats worse than previous ones
    /// @return A double representing the information gain
    [[nodiscard]] static double CalculateInformationGain(const ValueLine &values, double priorEntropy) {
        if (values.empty())
            return 0;

        double bestGain = 0;

        for (auto iter = values.begin(); std::next(iter, 1) != values.end(); iter++) {
            const double value = iter->first;
            const double nextValue = std::next(iter, 1)->first;
            const double splitPoint = value + (nextValue - value) / 2;
            const double splitEntropy = CalculateSplitEntropy(values, splitPoint);
            const double infoGain = priorEntropy - splitEntropy;

            if (infoGain > bestGain)
                bestGain = infoGain;
        }

        return bestGain;
    }
}

#endif //FEATUREEXTRACTION_INFORMATIONGAIN_H
