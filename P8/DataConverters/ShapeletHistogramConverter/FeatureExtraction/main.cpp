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

    id = Logger::Begin("Generating Data based on Split.");
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

    id = Logger::Begin("Writing Features to Files");
    std::unordered_map<int, std::vector<std::string>> paths;
    std::vector<std::string> trainPaths;
    std::vector<std::string> testPaths;
    for (const auto& seriesSet : mappedData) {
        const std::string dirPath = "data/" + std::to_string(seriesSet.first) + "/";
        for (uint i = 0; i < seriesSet.second.size(); i++) {
            const std::string filePath = dirPath + std::to_string(i);
            paths[seriesSet.first].push_back(filePath);
            if (trainMap.contains(seriesSet.first))
                trainPaths.push_back(filePath);
            else if (testMap.contains(seriesSet.first))
                testPaths.push_back(filePath);
            else
                throw std::logic_error("Unknown class " + std::to_string(seriesSet.first));
            const auto featureSeries = FeatureFinding::GenerateFeatureSeries(seriesSet.second.at(i), features);
            FileHanding::WriteFile(arguments.outPath + "/" + filePath, featureSeries);
        }
    }
    Logger::End(id);
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(trainPaths.begin(), trainPaths.end(), g);
    id = Logger::Begin("Writing Split Files");
    FileHanding::WriteFile(arguments.outPath + "/split/test.txt", testPaths);
    const uint valCount = (uint)(arguments.valtrainsplit * (double)trainPaths.size());
    std::vector<std::string> valPaths;
    for (uint i = 0; i < valCount; i++) {
        valPaths.push_back(trainPaths.back());
        trainPaths.pop_back();
    }
    FileHanding::WriteFile(arguments.outPath + "/split/train.txt", trainPaths);
    FileHanding::WriteFile(arguments.outPath + "/split/val.txt", valPaths);
    Logger::End(id);

    return 0;
}

int main(int argc, char** argv) {
    uint id = Logger::Begin("Parsing Arguments");
    auto arguments = ArgumentParsing::ParseArguments(argc, argv);
    Logger::End(id);

    return ConvertData(arguments);
}