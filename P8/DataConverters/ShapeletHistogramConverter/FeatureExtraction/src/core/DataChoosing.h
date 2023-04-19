#ifndef FEATUREEXTRACTION_DATACHOOSING_H
#define FEATUREEXTRACTION_DATACHOOSING_H

#include <unordered_map>
#include <set>
#include "misc/Constants.h"
#include "utilities/SeriesUtils.h"
#include "core/attributes/MinDist.h"

namespace DataChoosing {
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

    std::pair<std::vector<LabelledSeries>, std::vector<LabelledSeries>> Split(const std::vector<LabelledSeries> &series,
                                                                              double trainSplit) {
        const auto mappedData = SeriesUtils::ToMap(series);
        std::unordered_map<int, std::map<double, std::vector<Series>>> seriesScored;


        for (const auto &seriesSet : mappedData) {
            std::map<double, std::vector<Series>> setScores;
            for (const auto &s : seriesSet.second) {
                setScores[Score(seriesSet.second, s)].push_back(s);
            }
            seriesScored[seriesSet.first] = setScores;
        }

        std::unordered_map<int, std::vector<Series>> acceptable;
        std::unordered_map<int, std::vector<Series>> rejects;

        for (const auto &seriesSet : seriesScored) {
            std::vector<double> distances;
            for (const auto &s: seriesSet.second)
                distances.push_back(s.first);
            const double avg = std::accumulate(distances.begin(), distances.end(), (double) 0) / (double) distances.size();
            const double stdDiv = StdDiv(distances);
            for (const auto &ss : seriesSet.second) {
                const auto diff = std::abs(ss.first - avg);
                if (diff > stdDiv && false) {
                    for (const auto &s: ss.second)
                        rejects[seriesSet.first].push_back(s);
                } else {
                    for (const auto &s: ss.second)
                        acceptable[seriesSet.first].push_back(s);
                }
            }
        }
        std::vector<LabelledSeries> trainSet;
        std::vector<LabelledSeries> testSet;
        for (auto &seriesSet : acceptable) {
            std::shuffle(seriesSet.second.begin(), seriesSet.second.end(), g);
            uint i = 0;
            for (; i < trainSplit; i++)
                trainSet.emplace_back(seriesSet.first, seriesSet.second.at(i));
            for (; i < seriesSet.second.size(); i++)
                testSet.emplace_back(seriesSet.first, seriesSet.second.at(i));
        }
        for (const auto &seriesSet : rejects)
            for (const auto &s : seriesSet.second)
                testSet.emplace_back(seriesSet.first, s);

        return { trainSet, testSet };
    }
}

#endif //FEATUREEXTRACTION_DATACHOOSING_H
