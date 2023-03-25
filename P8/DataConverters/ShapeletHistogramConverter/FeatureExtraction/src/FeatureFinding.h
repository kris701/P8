#ifndef FEATUREEXTRACTION_FEATUREFINDING_H
#define FEATUREEXTRACTION_FEATUREFINDING_H

#include <memory>
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
#include <thread>
#include <future>

namespace FeatureFinding {
    const std::vector<Attribute*> attributes {
            new Frequency(0.04),
            new Frequency(0.4),
            new RelFrequency(0.04),
            new RelFrequency(0.4),
            new MinDist()
    }; // Intentionally not freed

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

    [[nodiscard]] static std::shared_ptr<Feature> FindOptimalFeature(const std::vector<LabelledSeries> &series, const std::vector<Series> &windows,
                                                            uint startIndex, uint endIndex) {
        if (series.size() < 2)
            throw std::logic_error("Cannot find features for less than two series.");
        if (windows.empty())
            throw std::logic_error("Missing windows.");

        const ClassCount counts = SeriesUtils::GetCount(series);
        const double currentEntropy = InformationGain::CalculateEntropy(counts);

        std::optional<Series> optimalShapelet;
        std::optional<Attribute*> optimalAttribute;
        double optimalGain = 0;

        for (uint i = startIndex; i < endIndex; i++) {
            const auto& window = windows.at(i);
            for (const auto &attribute: attributes) {
                const double gain = EvaluateWindow(currentEntropy, optimalGain, counts, attribute, series, window);

                if (!optimalShapelet.has_value() || gain > optimalGain) {
                    optimalShapelet = window;
                    optimalAttribute = attribute;
                    optimalGain = gain;
                }
            }
        }

        if (optimalGain > 0)
            return std::make_shared<Feature>( optimalShapelet.value(), optimalAttribute.value(), optimalGain );
        else
            return nullptr;
    }

    [[nodiscard]] std::shared_ptr<Feature> FindOptimalFeature(const std::vector<LabelledSeries> &series, const std::vector<Series> &windows) {
        const uint threadCount = std::thread::hardware_concurrency();

        std::thread threads[threadCount];
        std::shared_ptr<Feature> returns[threadCount];

        for (uint i = 0; i < threadCount; i++) {
            const uint startIndex = i * (windows.size() / threadCount);
            const uint endIndex = (i + 1) * (windows.size() / threadCount);
            threads[i] = std::thread([&, i] { returns[i] = FindOptimalFeature(series, windows, startIndex, endIndex); } );
        }

        for (uint i = 0; i < threadCount; i++)
            threads[i].join();

        std::optional<uint> optimalIndex;
        double optimalGain;

        for (uint i = 0; i < threadCount; i++)
            if (returns[i] != nullptr && (!optimalIndex.has_value() || returns[i]->gain > optimalGain)) {
                optimalIndex = i;
                optimalGain = returns[i]->gain;
            }

        if (optimalIndex.has_value())
            return returns[optimalIndex.value()];
        else
            return nullptr;
    }

    // *Not actually a tree
    [[nodiscard]] std::vector<Feature> GenerateFeatureTree(uint depth, const std::vector<LabelledSeries> &series, uint minWindowSize, uint maxWindowSize) {
        std::vector<Feature> features;
        if (depth == 0)
            return features;

        uint featureId = Logger::Begin("Generating Feature for Depth " + std::to_string(depth));

        const auto counts = SeriesUtils::GetCount(series);
        const auto windows = WindowGeneration::GenerateWindows(series, minWindowSize, maxWindowSize);
        const auto oFeature = FindOptimalFeature(series, windows);
        if (oFeature == nullptr)
            return features;
        const auto& feature = *oFeature;
        features.push_back(feature);

        uint splitId = Logger::Begin("Retrieving optimal split");
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

            for (const auto &f: GenerateFeatureTree(depth - 1, s, minWindowSize, maxWindowSize))
                features.push_back(f);
        }

        return features;
    }

    [[nodiscard]] std::vector<Feature> GenerateFeaturePairs(const std::unordered_map<int, std::vector<Series>> &data, uint minWindowSize, uint maxWindowSize) {
        std::vector<Feature> features;

        uint pairCount = 0;
        for (auto iter = data.begin(); iter != data.end(); iter++)
            for (auto iter2 = std::next(iter, 1); iter2 != data.end(); iter2++)
                pairCount++;

        using namespace indicators;

        printf("\n");

        ProgressBar bar{
                option::BarWidth{50},
                option::Start{"["},
                option::Fill{"="},
                option::Lead{">"},
                option::Remainder{" "},
                option::End{"]"},
                option::FontStyles{std::vector<FontStyle>{FontStyle::bold}},
                option::ShowElapsedTime{true},
                option::ShowRemainingTime{true},
                option::MaxProgress{pairCount}
        };

        show_console_cursor(false);

        for (auto iter = data.begin(); iter != data.end(); iter++)
            for (auto iter2 = std::next(iter, 1); iter2 != data.end(); iter2++) {
                const auto series = SeriesUtils::Mix((*iter).first, (*iter).second, (*iter2).first, (*iter2).second);
                const auto windows = WindowGeneration::GenerateWindows(series, minWindowSize, maxWindowSize);

                const auto feature = FindOptimalFeature(series, windows);
                if (feature == nullptr || feature->gain == 0)
                    continue;

                features.push_back(*feature);
                bar.tick();
            }

        show_console_cursor(true);

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
