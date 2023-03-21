#include <iostream>
#include <string>
#include "src/FileHanding.h"
#include "src/FeatureFinding.h"
#include "src/SeriesUtils.h"
#include "src/Logger.h"
#include "src/arguments/ArgumentParser.h"

int ConvertData(ArgumentParsing::Arguments arguments) {
    uint id = Logger::Begin("Reading Data");
    const auto data = SeriesUtils::Combine(
            FileHanding::ReadCSV(arguments.trainPath, "\t"),
            FileHanding::ReadCSV(arguments.testPath, "\t")
            );
    const auto mappedData = SeriesUtils::ToMap(data);
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
    std::vector<Feature> features = FeatureFinding::GenerateFeatureTree(arguments.depth, trainData, SeriesUtils::GetCount(trainData), arguments.minWindowSize, arguments.maxWindowSize);
    Logger::End(id);

    id = Logger::Begin("Generating Feature Points");
    const auto trainFeatures = FeatureFinding::GenerateFeatureSeries(trainData, features);
    const auto testFeatures = FeatureFinding::GenerateFeatureSeries(testData, features);
    const auto valFeatures = FeatureFinding::GenerateFeatureSeries(valData, features);
    Logger::End(id);

    id = Logger::Begin("Writing Features to Files");
    const auto featurePath = arguments.outPath + "/data/";
    const auto trainFiles = FileHanding::WriteToFiles(featurePath, trainFeatures);
    const auto testFiles = FileHanding::WriteToFiles(featurePath, testFeatures);
    const auto valFiles = FileHanding::WriteToFiles(featurePath, valFeatures);
    Logger::End(id);

    id = Logger::Begin("Writing Split Files");
    const auto splitPath = arguments.outPath + "/split/";
    FileHanding::WriteFile(splitPath + "train.txt", FileHanding::RemoveSubPath(arguments.outPath, trainFiles));
    FileHanding::WriteFile(splitPath + "test.txt", FileHanding::RemoveSubPath(arguments.outPath, testFiles));
    FileHanding::WriteFile(splitPath + "val.txt", FileHanding::RemoveSubPath(arguments.outPath, valFiles));
    Logger::End(id);

    return 0;
}

int main(int argc, char** argv) {
    uint id = Logger::Begin("Parsing Arguments");
    auto arguments = ArgumentParsing::ParseArguments(argc, argv);
    Logger::End(id);

    return ConvertData(arguments);
}