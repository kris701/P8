#ifndef FEATUREEXTRACTION_FEATUREFINDING_H
#define FEATUREEXTRACTION_FEATUREFINDING_H

#include <stdexcept>
#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>
#include "Types.h"
#include "SeriesUtils.h"
#include "InformationGain.h"
#include "Attributes.h"
#include "WindowGeneration.h"
#include "Logger.h"
#include "../include/indicators/cursor_control.hpp"
#include "../include/indicators/progress_bar.hpp"

namespace FeatureFinding {
    [[nodiscard]] double EvaluateWindow(double priorEntropy, double bestScore, const ClassCount &counts, Attribute attribute,
                                        const std::vector<LabelledSeries> &series, const Series &window) {
        std::map<double, ClassCount> valueCount; // At the given value, how many of each class

        ClassCount diff { counts };
        for (const auto &s : series) {
            valueCount[Attributes::GenerateValue(s.series, window, attribute)][s.label]++;
            diff.at(s.label)--;

            if (valueCount.size() < 2) // No split point at single value
                continue;

            // Early entropy pruning
            const double optimalSplitPoint = InformationGain::GetOptimalSplitPoint(valueCount);
            const auto split = InformationGain::GetSplit(valueCount, optimalSplitPoint);
            for (int i = 0; i < MAX_CLASSES; i++) // Adds the rest of the values optimally
                valueCount[split.first[i] > split.second[i]][i] += diff[i];

            const double optimalGain = InformationGain::CalculateInformationGain(valueCount, priorEntropy);
            if (optimalGain < bestScore)
                return 0;
            for (int i = 0; i < MAX_CLASSES; i++)
                valueCount[split.first[i] > split.second[i]][i] -= diff[i];
        }

        return InformationGain::CalculateInformationGain(valueCount, priorEntropy);
    }

    [[nodiscard]] Feature FindOptimalFeature(const std::vector<LabelledSeries> &series, const std::vector<Series> &windows) {
        if (series.size() < 2)
            throw std::logic_error("Cannot find features for less than two series.");
        if (windows.empty())
            throw std::logic_error("Missing windows.");

        const ClassCount counts = SeriesUtils::GetCount(series);
        const double currentEntropy = InformationGain::CalculateEntropy(counts);

        std::optional<Series> optimalShapelet;
        std::optional<Attribute> optimalAttribute;
        double optimalGain = 0;

        indicators::show_console_cursor(false);
        using namespace indicators;
        ProgressBar bar{
                option::BarWidth{80},
                option::Start{"["},
                option::Fill{"="},
                option::Lead{">"},
                option::Remainder{" "},
                option::End{" ]"},
                option::ForegroundColor{Color::white},
                option::FontStyles{
                    std::vector<FontStyle>{FontStyle::bold}},
                option::ShowElapsedTime{true}, option::ShowRemainingTime{true},
                option::MaxProgress{windows.size()}
        };

        for (uint i = 0; i < windows.size(); i++) {
            const auto& window = windows.at(i);
            for (const auto &attribute: ATTRIBUTES) {
                const double gain = EvaluateWindow(currentEntropy, optimalGain, counts, attribute, series, window);

                if (!optimalShapelet.has_value() || gain > optimalGain) {
                    optimalShapelet = window;
                    optimalAttribute = attribute;
                    optimalGain = gain;
                }
            }
            if (i % 1000 == 0) {
                bar.set_option(option::PostfixText{
                        std::to_string(i) + "/" + std::to_string(windows.size())
                    });
                bar.set_progress(i);
            }
        }
        indicators::show_console_cursor(true);
        bar.mark_as_completed();


        return Feature(optimalShapelet.value(), optimalAttribute.value());
    }
    
    // *Not actually a tree
    [[nodiscard]] std::vector<Feature> GenerateFeatureTree(int depth, const std::vector<LabelledSeries> &series, const ClassCount &counts, uint minWindowSize, uint maxWindowSize) {
        std::vector<Feature> features;
        if (depth == 0)
            return features;

        uint featureId = Logger::Begin("Generating Feature for Depth " + std::to_string(depth));

        const auto windows = WindowGeneration::GenerateWindows(series, minWindowSize, maxWindowSize);
        const auto feature = FindOptimalFeature(series, windows);
        features.push_back(feature);

        uint splitId = Logger::Begin("Retriving optimal split");
        const double splitPoint = InformationGain::GetOptimalSplitPoint(Attributes::GenerateValues(series, feature.shapelet, feature.attribute));
        const auto split = Attributes::SplitSeries(series, feature.attribute, feature.shapelet, splitPoint);
        Logger::End(splitId);

        Logger::End(featureId);

        for (const auto &s : { split.below, split.above }) {
            if (s.size() < 2)
                continue;
            const auto tempCounts = SeriesUtils::GetCount(s);

            bool harmonious = true; // All elements in either side of split, or it is empty
            for (uint i = 0; i < MAX_CLASSES; i++)
                if (tempCounts[i] != 0 && tempCounts[i] != counts[i]) {
                    harmonious = false;
                    break;
                }
            if (harmonious)
                continue;

            for (const auto &f: GenerateFeatureTree(depth - 1, s, tempCounts, minWindowSize, maxWindowSize))
                features.push_back(f);
        }

        return features;
    }

    std::vector<Feature> GenerateFeaturePairs(const std::unordered_map<int, std::vector<Series>> &trainData, const std::unordered_map<int, std::vector<Series>> &testData, uint minWindowSize, uint maxWindowSize) {
        std::vector<Feature> features;

        std::random_device rd;
        std::mt19937 g(rd());

        for (const auto &trainSet : trainData) {
            for (const auto &testSet : testData) {
                uint pairId = Logger::Begin("Generating feature for (" + std::to_string(trainSet.first) + "," + std::to_string(testSet.first) + ")");
                std::vector<LabelledSeries> series;
                for (const auto &seriesSet : { trainSet, testSet })
                    for (const auto &s : seriesSet.second)
                        series.push_back(LabelledSeries(seriesSet.first, s));
                std::shuffle(series.begin(), series.end(), g);
                
                const auto windows = WindowGeneration::GenerateWindows(series, minWindowSize, maxWindowSize);

                features.push_back(FindOptimalFeature(series, windows));
                Logger::End(pairId);
            }
        }

        return features;
    }

    std::vector<double> GenerateFeatureSeries(const Series &series, const std::vector<Feature> &features) {
        std::vector<double> featureSeries;

        for (const auto &feature : features)
            featureSeries.push_back(Attributes::GenerateValue(series, feature.shapelet, feature.attribute));

        return featureSeries;
    }
}

#endif //FEATUREEXTRACTION_FEATUREFINDING_H
