#include <iostream>
#include <string>
#include "src/FileHanding.h"
#include "src/FeatureFinding.h"
#include "src/SeriesUtils.h"
#include "src/Logger.h"
#include "src/ArgumentParser.h"
#include "src/FeatureUtils.h"

int ConvertData(ArgumentParsing::Arguments arguments) {
    uint id = Logger::Begin("Reading Data");
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
    const auto trainSplit = SeriesUtils::Split(valSplit.second, arguments.split);
    const auto trainData = trainSplit.first;
    const auto testData = trainSplit.second;
    const auto trainMap = SeriesUtils::ToMap(trainData);
    const auto testMap = SeriesUtils::ToMap(testData);
    Logger::End(id);

    id = Logger::Begin("Generating Feature Set");
    auto features = FeatureFinding::GenerateFeaturesFromSamples(trainMap, arguments.minWindowSize, arguments.maxWindowSize);
    Logger::End(id);

    id = Logger::Begin("Generating Feature Points");
    const auto trainFeatures = FeatureFinding::GenerateFeatureSeries(trainData, features);
    const auto testFeatures = FeatureFinding::GenerateFeatureSeries(testData, features);
    const auto valFeatures = FeatureFinding::GenerateFeatureSeries(valData, features);
    Logger::End(id);

    id = Logger::Begin("Writing Feature Series to Files");
    const auto featureSeriesPath = arguments.outPath + "data/";
    const auto trainFiles = FileHanding::WriteToFiles(featureSeriesPath, trainFeatures);
    const auto testFiles = FileHanding::WriteToFiles(featureSeriesPath, testFeatures);
    const auto valFiles = FileHanding::WriteToFiles(featureSeriesPath, valFeatures);
    Logger::End(id);

    id = Logger::Begin("Writing Split Files");
    const auto splitPath = arguments.outPath + "split/";
    FileHanding::WriteFile(splitPath + "train.txt", FileHanding::RemoveSubPath(arguments.outPath, trainFiles));
    FileHanding::WriteFile(splitPath + "test.txt", FileHanding::RemoveSubPath(arguments.outPath, testFiles));
    FileHanding::WriteFile(splitPath + "val.txt", FileHanding::RemoveSubPath(arguments.outPath, valFiles));
    Logger::End(id);

    id = Logger::Begin("Writing Feature Files");
    const auto featurePath = arguments.outPath + "features/";
    const auto shapeletPath = featurePath + "shapelets/";
    const auto shapelets = FeatureUtils::RetrieveShapelets(features);
    const auto shapeletFiles = FileHanding::RemoveSubPath(featurePath, FileHanding::WriteToFiles(shapeletPath, shapelets));
    FileHanding::WriteCSV(featurePath + "features.csv",
                          FeatureUtils::FeatureHeader(),
                          FeatureUtils::FeatureCSV(features, shapeletFiles));
    Logger::End(id);

    return 0;
}

int main(int argc, char** argv) {
    uint id = Logger::Begin("Parsing Arguments");
    auto arguments = ArgumentParsing::ParseArguments(argc, argv);
    Logger::End(id);

    return ConvertData(arguments);
}