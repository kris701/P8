#ifndef FEATUREEXTRACTION_DATAPURGE_H
#define FEATUREEXTRACTION_DATAPURGE_H

#include <unordered_map>
#include <set>
#include "misc/Constants.h"
#include "core/attributes/MinDist.h"
#include "types/SeriesMap.h"

namespace DataPurge {
    struct Results {
        const SeriesMap acceptable;
        const SeriesMap rejects;
        Results(SeriesMap acceptable, SeriesMap rejects) : acceptable(std::move(acceptable)), rejects(std::move(rejects)) {}
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

    Results Purge(const SeriesMap &series) {
        const auto mappedData = SeriesMap(series);
        std::unordered_map<uint, std::map<double, std::vector<Series>>> seriesScored;

        for (const auto &seriesSet : mappedData) {
            std::map<double, std::vector<Series>> setScores;
            for (const auto &s : seriesSet.second) {
                setScores[Score(seriesSet.second, s)].push_back(s);
            }
            seriesScored[seriesSet.first] = setScores;
        }

        SeriesMap acceptable;
        SeriesMap rejects;

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
                        rejects[seriesSet.first].push_back(s);
                        rejectCount++;
                    }
                } else {
                    for (const auto &s: ss.second)
                        acceptable[seriesSet.first].push_back(s);
                }
            }
            Logger::Info("Purged " + std::to_string(rejectCount) + " data points from class " + std::to_string(seriesSet.first));
        }
        Logger::Info("Purged " + std::to_string((double) rejects.size() / ((double) rejects.size() + (double) acceptable.size()) * 100.0) + "%");
        return { acceptable, rejects };
    }
}

#endif //FEATUREEXTRACTION_DATAPURGE_H
