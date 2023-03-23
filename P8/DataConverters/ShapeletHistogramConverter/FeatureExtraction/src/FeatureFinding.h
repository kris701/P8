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
#include "WindowGeneration.h"
#include "Logger.h"
#include "../include/indicators/cursor_control.hpp"
#include "../include/indicators/progress_bar.hpp"
#include "Feature.h"
#include "attributes/Frequency.h"
#include "attributes/MinDist.h"
#include "attributes/RelFrequency.h"

namespace FeatureFinding {
    [[nodiscard]] double EvaluateWindow(double priorEntropy, double bestScore, const ClassCount &counts, const Attribute *attribute,
                                        const std::vector<LabelledSeries> &series, const Series &window) {
        std::map<double, ClassCount> valueCount; // At the given value, how many of each class

        ClassCount diff { counts };
        for (const auto &s : series) {
            valueCount[attribute->GenerateValue(s.series, window)][s.label]++;
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

        if (valueCount.size() < 2) // Impossible to split a single point
            return 0;
        else
            return InformationGain::CalculateInformationGain(valueCount, priorEntropy);
    }

    [[nodiscard]] std::optional<Feature> FindOptimalFeature(const std::vector<LabelledSeries> &series, const std::vector<Series> &windows) {
        if (series.size() < 2)
            throw std::logic_error("Cannot find features for less than two series.");
        if (windows.empty())
            throw std::logic_error("Missing windows.");

        const std::vector<Attribute*> attributes {
            new Frequency(0.01),
            new Frequency(0.02),
            new Frequency(0.04),
            new Frequency(0.08),
            new Frequency(0.1),
            new Frequency(0.2),
            new Frequency(0.4),
            new Frequency(0.8),
            new RelFrequency(0.01),
            new RelFrequency(0.02),
            new RelFrequency(0.04),
            new RelFrequency(0.08),
            new RelFrequency(0.1),
            new RelFrequency(0.2),
            new RelFrequency(0.4),
            new RelFrequency(0.8),
            new MinDist()
        }; // Intentionally not freed

        const ClassCount counts = SeriesUtils::GetCount(series);
        const double currentEntropy = InformationGain::CalculateEntropy(counts);

        std::optional<Series> optimalShapelet;
        std::optional<Attribute*> optimalAttribute;
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
            for (const auto &attribute: attributes) {
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
        bar.mark_as_completed();
        indicators::show_console_cursor(true);

        if (optimalGain > 0)
            return Feature(optimalShapelet.value(), optimalAttribute.value(), optimalGain);
        else
            return std::optional<Feature>();
    }
    
    // *Not actually a tree
    [[nodiscard]] std::vector<Feature> GenerateFeatureTree(uint depth, const std::vector<LabelledSeries> &series, const ClassCount &counts, uint minWindowSize, uint maxWindowSize) {
        std::vector<Feature> features;
        if (depth == 0)
            return features;

        uint featureId = Logger::Begin("Generating Feature for Depth " + std::to_string(depth));

        const auto windows = WindowGeneration::GenerateWindows(series, minWindowSize, maxWindowSize);
        const auto oFeature = FindOptimalFeature(series, windows);
        if (!oFeature.has_value())
            return features;
        const auto& feature = oFeature.value();
        features.push_back(feature);

        uint splitId = Logger::Begin("Retriving optimal split");
        const double splitPoint = InformationGain::GetOptimalSplitPoint(feature.attribute->GenerateValues(series, feature.shapelet));
        const auto split = feature.attribute->SplitSeries(series, feature.shapelet, splitPoint);
        Logger::End(splitId);

        Logger::End(featureId);

        for (const auto &s : { split.at(0), split.at(1) }) {
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

                const auto feature = FindOptimalFeature(series, windows);
                if (feature.has_value())
                    features.push_back(feature.value());
                Logger::End(pairId);
            }
        }

        return features;
    }

    std::vector<double> GenerateFeatureSeries(const Series &series, const std::vector<Feature> &features) {
        std::vector<double> featureSeries;

        for (const auto &feature : features)
            featureSeries.push_back(feature.attribute->GenerateValue(series, feature.shapelet));

        return featureSeries;
    }

    std::unordered_map<int, std::vector<std::vector<double>>> GenerateFeatureSeries
    (const std::vector<LabelledSeries> &series, const std::vector<Feature> &features) {
        std::unordered_map<int, std::vector<std::vector<double>>> featureSeriesSet;

        for (const auto &s : series)
            featureSeriesSet[s.label].push_back(GenerateFeatureSeries(s.series, features));

        return featureSeriesSet;
    }
}

#endif //FEATUREEXTRACTION_FEATUREFINDING_H
