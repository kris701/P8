#ifndef FEATUREEXTRACTION_DATAAUGMENTATION_H
#define FEATUREEXTRACTION_DATAAUGMENTATION_H

#include <stdexcept>
#include "misc/Constants.hpp"

namespace DataAugmentation {
    Series Smooth(Series series, uint degree) {
        Series smoothSeries;

        for (int i = 0; i < series.size(); i++) {
            uint count = 0;
            double total = 0;
            for (int t = std::max(0, i - (int) degree); t < std::min((uint) series.size(), i + degree); t++) {
                total += series.at(t);
                count++;
            }
            smoothSeries.push_back(total / count);
        }

        return smoothSeries;
    }

    SeriesMap Smooth(const SeriesMap &series, uint degree) {
        SeriesMap smoothedSeries;

        for (const auto &seriesSet : series)
            for (const auto &s : seriesSet.second)
            smoothedSeries[seriesSet.first].push_back(Smooth(s, degree));

        return smoothedSeries;
    }

    Series Noiseify(Series series, double amount) {
        Series noisySeries;

        for (double s : series) {
            const double change = s * amount;

            noisySeries.push_back(s + ((rand() % 2 == 0) ? change : -change));
        }

        return noisySeries;
    }

    SeriesMap Noiseify(SeriesMap series, double amount) {
        SeriesMap noisySeries;

        for (const auto &seriesSet : series)
            for (const auto &s : seriesSet.second)
                noisySeries[seriesSet.first].push_back(Noiseify(s, amount));

        return noisySeries;
    }

    SeriesMap Augment(const SeriesMap &original, bool deleteOriginal, uint smoothDegree, double noiseifyAmount) {
        SeriesMap newSeries;

        if (!deleteOriginal)
            newSeries.InsertAll(original);

        if (smoothDegree != 0)
            newSeries.InsertAll(Smooth(original, smoothDegree));

        if (noiseifyAmount != 0)
            newSeries.InsertAll(Noiseify(original, noiseifyAmount));

        return newSeries;
    }
}

#endif //FEATUREEXTRACTION_DATAAUGMENTATION_H
