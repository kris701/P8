#include <iostream>
#include <string>
#include "src/FileHanding.h"
#include "src/FeatureFinding.h"
#include "src/SeriesUtils.h"
#include "src/Logger.h"
#include "src/arguments/ArgumentParser.h"

int ConvertData(ArgumentParsing::Arguments arguments) {
    uint id = Logger::Begin("Reading Data");
    const auto tempTrainData = FileHanding::ReadCSV(arguments.trainPath, "\t");
    const auto tempTestData = FileHanding::ReadCSV(arguments.testPath, "\t");
    Logger::End(id);

    id = Logger::Begin("Generating Data based on Split.");
    std::unordered_map<int, std::vector<Series>> mappedData;
    std::unordered_set<int> classes;
    for (const auto& seriesSet : { tempTrainData, tempTestData })
        for (const auto& s : seriesSet) {
            mappedData[s.label].push_back(s.series);
            classes.emplace(s.label);
        }

    const uint trainClassCount = (uint)(arguments.split * (double)mappedData.size());

    std::unordered_map<int, std::vector<Series>> trainMap;
    std::unordered_map<int, std::vector<Series>> testMap;
    for (uint i = 0; i < trainClassCount; i++) {
        const auto map = (*std::next(mappedData.begin(), i));
        for (const auto& s : map.second)
            trainMap[map.first].push_back(s);
    }
    for (uint i = trainClassCount; i < classes.size(); i++) {
        const auto map = (*std::next(mappedData.begin(), i));
        for (const auto& s : map.second)
            testMap[map.first].push_back(s);
    }

    std::vector<LabelledSeries> trainData;
    for (const auto& seriesSet : trainMap)
        for (const auto& s : seriesSet.second)
            trainData.emplace_back(seriesSet.first, s);

    std::vector<LabelledSeries> testData;
    for (const auto& seriesSet : testMap)
        for (const auto& s : seriesSet.second)
            testData.emplace_back(seriesSet.first, s);

    Logger::End(id);

    id = Logger::Begin("Generating Feature Set");
    std::vector<Feature> features = FeatureFinding::GenerateFeatureTree(arguments.depth, trainData, SeriesUtils::GetCount(trainData), arguments.minWindowSize, arguments.maxWindowSize);
    Logger::End(id);

    id = Logger::Begin("Generating Feature Pairs");
    for (const auto& feature : FeatureFinding::GenerateFeaturePairs(trainMap, testMap, arguments.minWindowSize, arguments.maxWindowSize))
        features.push_back(feature);
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