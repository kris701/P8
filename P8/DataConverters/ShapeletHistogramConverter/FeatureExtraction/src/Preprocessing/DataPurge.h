#ifndef FEATUREEXTRACTION_DATAPURGE_H
#define FEATUREEXTRACTION_DATAPURGE_H

#include <unordered_map>
#include <set>
#include "misc/Constants.h"
#include "utilities/SeriesUtils.h"
#include "core/attributes/MinDist.h"

namespace DataPurge {
    struct Results {
        const std::vector<LabelledSeries> acceptable;
        const std::vector<LabelledSeries> rejects;
        Results(std::vector<LabelledSeries> acceptable, std::vector<LabelledSeries> rejects) : acceptable(std::move(acceptable)), rejects(std::move(rejects)) {}
    };

    double StdDiv(const std::vector<double> &vec) {
        const double mean = std::accumulate(vec.begin(), vec.end(), (double) 0) / (double) vec.size();
        return std::sqrt(
                std::accumulate(vec.begin(), vec.end(), (double) 0, [mean](double acc, const double &e) {
                    return acc + std::pow(e - mean, 2);
                })
                / (double) vec.size()
                );
    }

    static double Score(const std::vector<Series> &allSeries, const Series &series) {
        double totalDist = 0;

        MinDist tempMinDist = MinDist();

        for (const auto &s : allSeries)
            totalDist += tempMinDist.GenerateValue(s, series);

        return totalDist / (double) allSeries.size();
    }

    Results Purge(const std::vector<LabelledSeries> &series) {
        const auto mappedData = SeriesUtils::ToMap(series);
        std::unordered_map<int, std::map<double, std::vector<Series>>> seriesScored;

        for (const auto &seriesSet : mappedData) {
            std::map<double, std::vector<Series>> setScores;
            for (const auto &s : seriesSet.second) {
                setScores[Score(seriesSet.second, s)].push_back(s);
            }
            seriesScored[seriesSet.first] = setScores;
        }

        std::vector<LabelledSeries> acceptable;
        std::vector<LabelledSeries> rejects;

        for (const auto &seriesSet : seriesScored) {
            std::vector<double> distances;
            for (const auto &s: seriesSet.second)
                distances.push_back(s.first);
            const double avg = std::accumulate(distances.begin(), distances.end(), (double) 0) / (double) distances.size();
            const double stdDiv = StdDiv(distances);
            uint rejectCount = 0;
            for (const auto &ss : seriesSet.second) {
                const auto diff = std::abs(ss.first - avg);
                if (diff > stdDiv) {
                    for (const auto &s: ss.second) {
                        rejects.emplace_back(seriesSet.first, s);
                        rejectCount++;
                    }
                } else {
                    for (const auto &s: ss.second)
                        acceptable.emplace_back(seriesSet.first, s);
                }
            }
            Logger::Info("Purged " + std::to_string(rejectCount) + " data points from class " + std::to_string(seriesSet.first));
        }
        return { acceptable, rejects };
    }
}

#endif //FEATUREEXTRACTION_DATAPURGE_H
