#ifndef FEATUREEXTRACTION_FEATUREFINDING_H
#define FEATUREEXTRACTION_FEATUREFINDING_H

#include <memory>
#include <stdexcept>
#include <random>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <vector>
#include "misc/Constants.h"
#include "utilities/SeriesUtils.h"
#include "InformationGain.h"
#include "WindowGeneration.h"
#include "IO/Logger.h"
#include "../../include/indicators/indicators.hpp"
#include "types/Feature.h"
#include "types/attributes/Frequency.h"
#include "types/attributes/MinDist.h"
#include "types/attributes/RelFrequency.h"
#include <thread>
#include <future>

namespace FeatureFinding {
    const std::vector<Attribute*> attributes {
            new Frequency(0.04),
            new Frequency(0.4),
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

    /// Evaluates each window in \p windows, in combination with each attribute
    /// \return Returns the feature with highest information gain. Can be nullptr if no valid feature found.
    static std::shared_ptr<Feature> FindOptimalFeature(const std::vector<LabelledSeries> &series,
                                   const std::vector<Series> &windows) {
        if (series.size() < 2)
            throw std::logic_error("Cannot find features for less than two series.");
        if (windows.empty())
            throw std::logic_error("Missing windows.");

        const auto counts = SeriesUtils::GetCount(series);
        const auto entropy = InformationGain::CalculateEntropy(counts);
        double optimalGain = 0;
        std::shared_ptr<Feature> optimalFeature = nullptr;
        for (const auto & window : windows) {
            for (const auto &attribute: attributes) {
                const double gain = EvaluateWindow(entropy, optimalGain, counts, attribute, series, window);
                if (gain > optimalGain)
                    if (optimalFeature == nullptr || gain > optimalGain) {
                        optimalGain = gain;
                        optimalFeature = std::make_shared<Feature>(Feature(window, attribute, gain));
                    }
            }
        }
        return optimalFeature;
    }

    /// Generates a number of features based upon samples from each class
    /// \param seriesMap Data to generate features from, where the key is class and value is series set
    /// \param featureCount How many features to generate
    /// \param sampleSize How many samples to take from each class for each feature. If \p sampleSize is 3, it takes
    /// a 3 samples from each class.
    /// \return A list of features. Can be empty if no valid features are found.
    [[nodiscard]] std::vector<Feature> GenerateFeaturesFromSamples(const std::unordered_map<int, std::vector<Series>> &seriesMap,
                                                                   uint minWindowSize, uint maxWindowSize,
                                                                   uint featureCount, uint sampleSize) {
        std::vector<Feature> features;


        using namespace indicators;
        show_console_cursor(false);
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
                option::MaxProgress{featureCount}
        };
        printf("\n");

        for (uint i = 0; i < featureCount; i++) {
            bar.print_progress();
            std::vector<LabelledSeries> samples;

            // Retrieve n samples from each class
            for (const auto &seriesSet : seriesMap) {
                std::vector<Series> tempSamples;
                const uint tempSampleSize = std::min(sampleSize, (uint) seriesSet.second.size()); // If sampleSize is larger than the number of available data points, set to max possible
                std::sample(seriesSet.second.begin(), seriesSet.second.end(), std::back_inserter(tempSamples), tempSampleSize, rd);
                for (const auto &sample : tempSamples)
                    samples.emplace_back(seriesSet.first, sample);
            }

            // Generate feature based on samples
            const auto feature = FindOptimalFeature(samples, WindowGeneration::GenerateWindows(samples, minWindowSize, maxWindowSize));
            if (feature != nullptr)
                features.push_back(*feature);
            bar.tick();
        }
        show_console_cursor(true);

        return features;
    }
}

#endif //FEATUREEXTRACTION_FEATUREFINDING_H