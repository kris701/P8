#ifndef FEATUREEXTRACTION_DATAAUGMENTATION_H
#define FEATUREEXTRACTION_DATAAUGMENTATION_H

#include <stdexcept>
#include "misc/Constants.h"

namespace DataAugmentation {
    LabelledSeries Smooth(LabelledSeries series, uint degree) {
        Series smoothSeries;

        for (int i = 0; i < series.series.size(); i++) {
            uint count = 0;
            double total = 0;
            for (int t = std::max(0, i - (int) degree); t < std::min((uint) series.series.size(), i + degree); t++) {
                total += series.series.at(t);
                count++;
            }
            smoothSeries.push_back(total / count);
        }

        return { series.label, smoothSeries };
    }

    std::vector<LabelledSeries> Smooth(std::vector<LabelledSeries> series, uint degree) {
        std::vector<LabelledSeries> smoothedSeries;

        for (const auto &s : series)
            smoothedSeries.push_back(Smooth(s, degree));

        return smoothedSeries;
    }

    LabelledSeries Noiseify(LabelledSeries series, double amount) {
        Series smoothSeries;

        for (double s : series.series) {
            const double change = s * amount;

            smoothSeries.push_back(s + ((rand() % 2 == 0) ? change : -change));
        }

        return { series.label, smoothSeries };
    }

    std::vector<LabelledSeries> Noiseify(std::vector<LabelledSeries> series, double amount) {
        std::vector<LabelledSeries> smoothedSeries;

        for (const auto &s : series)
            smoothedSeries.push_back(Noiseify(s, amount));

        return smoothedSeries;
    }

    std::vector<LabelledSeries> Augment(std::vector<LabelledSeries> original, bool deleteOriginal, uint smoothDegree, double noiseifyAmount) {
        std::vector<LabelledSeries> newSeries;

        if (!deleteOriginal)
            newSeries.insert(newSeries.end(), original.begin(), original.end());

        if (smoothDegree != 0) {
            const auto smoothed = Smooth(original, smoothDegree);
            newSeries.insert(newSeries.end(), smoothed.begin(), smoothed.end());
        }

        if (noiseifyAmount != 0) {
            const auto noised = Noiseify(original, noiseifyAmount);
            newSeries.insert(newSeries.end(), noised.begin(), noised.end());
        }

        return newSeries;
    }
}

#endif //FEATUREEXTRACTION_DATAAUGMENTATION_H
