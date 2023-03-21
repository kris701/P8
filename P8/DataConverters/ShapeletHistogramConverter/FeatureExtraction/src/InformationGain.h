#ifndef FEATUREEXTRACTION_INFORMATIONGAIN_H
#define FEATUREEXTRACTION_INFORMATIONGAIN_H

#include <cmath>
#include <map>
#include <numeric>
#include <optional>
#include "Types.h"
#include "SeriesUtils.h"

namespace InformationGain {
    [[nodiscard]] static double CalculateEntropy(uint total, const ClassCount &counts) {
        double entropy = 0;
        for (int i = 0; i < MAX_CLASSES; i++) {
            const double prob = (double) counts.at(i) / total;
            if (prob > 0)
                entropy += prob * (std::log2(prob));
        }
        return -entropy;
    }

    [[nodiscard]] static inline double CalculateEntropy(const ClassCount &counts) {
        return CalculateEntropy(std::accumulate(counts.begin(), counts.end(), (uint) 0), counts);
    }

    [[nodiscard]] static inline double CalculateEntropy(const std::vector<LabelledSeries> &series) {
        return CalculateEntropy(series.size(), SeriesUtils::GetCount(series));
    }

    [[nodiscard]] static std::pair<ClassCount , ClassCount> GetSplit (const std::map<double, ClassCount> &values, double splitPoint) {
        ClassCount lowerCount { 0 };
        ClassCount upperCount { 0 };
        for (const auto &frequency : values)
            if (frequency.first < splitPoint)
                for (int i = 0; i < MAX_CLASSES; i++)
                    lowerCount[i] += frequency.second[i];
            else
                for (int i = 0; i < MAX_CLASSES; i++)
                    upperCount[i] += frequency.second[i];
        return std::make_pair(lowerCount, upperCount);
    }

    [[nodiscard]] static double CalculateSplitEntropy(const std::map<double, ClassCount> &values, double splitPoint) {
        const auto split = GetSplit(values, splitPoint);
        const uint lowerTotal = std::accumulate(split.first.begin(), split.first.end(), (uint) 0);
        const uint upperTotal = std::accumulate(split.second.begin(), split.second.end(), (uint) 0);

        const uint total = lowerTotal + upperTotal;
        const double lowerEntropy = CalculateEntropy(total, split.first);
        const double upperEntropy = CalculateEntropy(total, split.second);

        const double lowerProb = (double) lowerTotal / total;
        const double upperProb = (double) upperTotal / total;

        return lowerEntropy * lowerProb + upperEntropy * upperProb;
    }

    [[nodiscard]] static double GetOptimalSplitPoint(const std::map<double, ClassCount> &matchFrequency) {
        if (matchFrequency.size() < 2)
            throw std::logic_error("Trying to split a single point");
        std::optional<double> bestPoint;
        double bestEntropy;

        for (auto iter = matchFrequency.begin(); iter != matchFrequency.end() && std::next(iter, 1) != matchFrequency.end(); iter++) {
            const double splitPoint = iter->first + (std::next(iter, 1)->first - iter->first) / 2;
            const double splitEntropy = CalculateSplitEntropy(matchFrequency, splitPoint);

            if (!bestPoint.has_value() || splitEntropy < bestEntropy) {
                bestPoint = splitPoint;
                bestEntropy = splitEntropy;
            }
        }

        return bestPoint.value();
    }

    [[nodiscard]] static double CalculateInformationGain(const std::map<double, ClassCount> &matchFrequency, double priorEntropy) {
        double bestGain = 0;

        for (auto iter = matchFrequency.begin(); iter != matchFrequency.end() && std::next(iter, 1) != matchFrequency.end(); iter++) {
            const double splitPoint = iter->first + (std::next(iter, 1)->first - iter->first) / 2;
            const double splitEntropy = CalculateSplitEntropy(matchFrequency, splitPoint);
            const double infoGain = priorEntropy - splitEntropy;

            if (infoGain > bestGain)
                bestGain = infoGain;
        }

        return bestGain;
    }
}

#endif //FEATUREEXTRACTION_INFORMATIONGAIN_H
