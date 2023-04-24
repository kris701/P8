#ifndef FEATUREEXTRACTION_EVALUATION_H
#define FEATUREEXTRACTION_EVALUATION_H

#include <memory>
#include <algorithm>
#include <iterator>
#include <vector>
#include "misc/Constants.h"
#include "utilities/SeriesUtils.h"
#include "utilities/ClassCountUtils.h"
#include "core/InformationGain.h"
#include "core/WindowGeneration.h"
#include "types/Feature.h"
#include "core/attributes/Attribute.h"

namespace FeatureFinding::Evaluation {
    [[nodiscard]] static bool EntropyPrune(double bestScore, double priorEntropy,
                                           std::map<double, ClassCount> &valueCount, ClassCount &diff) {
        if (valueCount.size() < 2) // No split point at single value
            return false;

        const double optimalSplitPoint = InformationGain::GetOptimalSplitPoint(valueCount);
        const auto split = ClassCountUtils::GetSplit(valueCount, optimalSplitPoint);
        for (int i = 0; i < MAX_CLASSES; i++) // Adds the rest of the values optimally
            valueCount[split.first[i] > split.second[i]][i] += diff[i];

        const double optimalGain = InformationGain::CalculateInformationGain(valueCount, priorEntropy);
        if (optimalGain < bestScore)
            return true;
        for (int i = 0; i < MAX_CLASSES; i++)
            valueCount[split.first[i] > split.second[i]][i] -= diff[i];
        return false;
    }

    [[nodiscard]] static double EvaluateWindow(double priorEntropy, double bestScore, const ClassCount &counts,
                                               const std::shared_ptr<Attribute> attribute,
                                               const std::vector<LabelledSeries> &series, const Series &window) {
        std::map<double, ClassCount> valueCount; // At the given value, how many of each class

        ClassCount diff { counts };
        for (const auto &s : series) {
            valueCount[attribute->GenerateValue(s.series, window)][s.label]++;
            diff[s.label]--;

            if (EntropyPrune(bestScore, priorEntropy, valueCount, diff))
                return 0;
        }

        if (valueCount.size() < 2) // Impossible to split a single point
            return 0;
        else
            return InformationGain::CalculateInformationGain(valueCount, priorEntropy);
    }
}

#endif //FEATUREEXTRACTION_EVALUATION_H
