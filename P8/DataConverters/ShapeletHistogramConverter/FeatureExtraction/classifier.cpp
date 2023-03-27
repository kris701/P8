#include <iostream>
#include <string>
#include <set>
#include "src/FileHanding.h"
#include "src/FeatureFinding.h"
#include "src/SeriesUtils.h"
#include "src/Logger.h"
#include "src/ArgumentParser.h"
#include "src/FeatureUtils.h"
#include "src/Types.h"
#include "src/Classification.h"

int main(int argc, char** argv) {
    uint id = Logger::Begin("Parsing Arguments");
    auto arguments = ArgumentParsing::ParseArguments(argc, argv);
    Logger::End(id);

    id = Logger::Begin("Reading Data");
    auto data = SeriesUtils::Combine(
            FileHanding::ReadCSV(arguments.trainPath, "\t"),
            FileHanding::ReadCSV(arguments.testPath, "\t")
    );

    const auto mappedData = SeriesUtils::ToMap(data);
    Logger::End(id);

    id = Logger::Begin("Normalizing Data");
    SeriesUtils::MinMaxNormalize(data);
    Logger::End(id);

    id = Logger::Begin("Shuffling Data");
    std::shuffle(data.begin(), data.end(), g);
    Logger::End(id);

    id = Logger::Begin("Splitting Data");
    const auto valSplit = SeriesUtils::Split(data, arguments.valtrainsplit);
    const auto valData = valSplit.first;
    if (!valData.empty())
        throw std::logic_error("Val data is not used, please remove it.");
    const auto trainSplit = SeriesUtils::Split(valSplit.second, arguments.split);
    const auto trainData = trainSplit.first;
    const auto testData = trainSplit.second;
    const auto trainMap = SeriesUtils::ToMap(trainData);
    const auto testMap = SeriesUtils::ToMap(testData);
    Logger::End(id);

    id = Logger::Begin("Generating Feature Set");
    auto features = FeatureFinding::GenerateFeatureTree(arguments.depth, trainData, arguments.minWindowSize, arguments.maxWindowSize);
    Logger::End(id);

    id = Logger::Begin("Generating Feature Pairs");
    const auto pairFeatures = FeatureFinding::GenerateFeaturePairs(trainMap, arguments.minWindowSize, arguments.maxWindowSize);
    for (const auto &feature : pairFeatures)
        features.emplace_back(feature.shapelet, feature.attribute, feature.gain, feature.classes);
    Logger::End(id);

    id = Logger::Begin("Generating Feature Points");
    const auto trainFeatures = FeatureFinding::GenerateFeatureSeries(trainData, features);
    Logger::End(id);

    printf("k-Nearest Neighbours:\n");

    uint correct = 0;
    uint incorrect = 0;
    for (const auto &s : testData) {
        int guess = Classification::KNearestNeighbours(features, trainFeatures, s.series, 1);

        if (guess == s.label)
            correct++;
        else
            incorrect++;
    }

    printf("k - %d: Accuracy -  %f\n", 1, (double) correct / (double) (correct + incorrect));

    printf("Weigthed k-Nearest Neighbours:\n");

    correct = 0;
    incorrect = 0;
    for (const auto &s : testData) {
        int guess = Classification::WeightedKNearestNeighbours(features, trainFeatures, s.series, 1);

        if (guess == s.label)
            correct++;
        else
            incorrect++;
    }

    printf("k - %d: Accuracy -  %f\n", 1, (double) correct / (double) (correct + incorrect));

    return 0;
}
