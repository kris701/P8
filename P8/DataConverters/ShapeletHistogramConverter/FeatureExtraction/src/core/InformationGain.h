#ifndef FEATUREEXTRACTION_INFORMATIONGAIN_H
#define FEATUREEXTRACTION_INFORMATIONGAIN_H

#include <cmath>
#include <map>
#include <numeric>
#include "misc/Constants.h"
#include "utilities/SeriesUtils.h"
#include "utilities/ClassCountUtils.h"

namespace InformationGain {
    /// <summary>
    /// Calculates the entropy between a set of classes.
    /// <para>Based on Definition 6 in <seealso cref="http://alumni.cs.ucr.edu/~lexiangy/Shapelet/Shapelet.pdf"/></para>
    /// </summary>
    /// <param name="total"></param>
    /// <param name="counts"></param>
    /// <returns></returns>
    [[nodiscard]] static double CalculateEntropy(uint total, const ClassCount &counts) {
        if (total == 0 || counts.size() == 0)
            throw std::exception("Cannot calculate entropy with zero values!");

        double entropy = 0;
        for (int i = 0; i < MAX_CLASSES; i++) {
            if (counts.at(i) > 0) {
                const double proportion = (double)counts.at(i) / total;
                entropy += (-proportion) * (std::log2(proportion));
            }
        }
        return entropy;
    }

    [[nodiscard]] static inline double CalculateEntropy(const ClassCount &counts) {
        return CalculateEntropy(ClassCountUtils::GetTotalClassCount(counts), counts);
    }

    [[nodiscard]] static double CalculateSplitEntropy(const std::map<double, ClassCount> &values, double splitPoint) {
        const auto split = ClassCountUtils::GetSplit(values, splitPoint);
        const uint lowerTotal = ClassCountUtils::GetTotalClassCount(split.first);
        const uint upperTotal = ClassCountUtils::GetTotalClassCount(split.second);

        const uint total = lowerTotal + upperTotal;
        const double lowerEntropy = CalculateEntropy(total, split.first);
        const double upperEntropy = CalculateEntropy(total, split.second);

        const double lowerProb = (double) lowerTotal / total;
        const double upperProb = (double) upperTotal / total;

        return lowerEntropy * lowerProb + upperEntropy * upperProb;
    }

    [[nodiscard]] static double GetOptimalSplitPoint(const std::map<double, ClassCount> &values) {
        if (values.size() < 2)
            throw std::exception("Trying to split a single point");
        double bestPoint = -1;
        double bestEntropy = DBL_MAX;

        for (auto iter = values.begin(); std::next(iter, 1) != values.end(); iter++) {
            const double thisValue = iter->first;
            const double nextValue = std::next(iter, 1)->first;
            const double splitPoint = thisValue + (nextValue - thisValue) / 2;
            const double splitEntropy = CalculateSplitEntropy(values, splitPoint);

            if (splitEntropy < bestEntropy) {
                bestPoint = splitPoint;
                bestEntropy = splitEntropy;
            }
        }

        if (bestPoint == -1)
            throw std::exception("No valid best point was found!");

        return bestPoint;
    }

    [[nodiscard]] static double CalculateInformationGain(const std::map<double, ClassCount> &values, double priorEntropy) {
        double bestGain = 0;

        for (auto iter = values.begin(); std::next(iter, 1) != values.end(); iter++) {
            const double thisValue = iter->first;
            const double nextValue = std::next(iter, 1)->first;
            const double splitPoint = thisValue + (nextValue - thisValue) / 2;
            const double splitEntropy = CalculateSplitEntropy(values, splitPoint);
            const double infoGain = priorEntropy - splitEntropy;

            if (infoGain > bestGain)
                bestGain = infoGain;
        }

        return bestGain;
    }
}

#endif //FEATUREEXTRACTION_INFORMATIONGAIN_H
