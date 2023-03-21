#include <iostream>
#include <string>
#include "include/cxxopts/cxxopts.hpp"
#include "src/FileHanding.h"
#include "src/FeatureFinding.h"
#include "src/SeriesUtils.h"
#include "src/Logger.h"

struct Arguments {
    std::string trainPath;
    std::string testPath;
    std::string outPath;
    double split = -1;
    double valtrainsplit = 0.3;
    uint minWindowSize = 2;
    uint maxWindowSize = 8;
};

Arguments ParseArguments (int argc, char **argv) {
    cxxopts::Options options("FeatureExtraction", "Extracts features from given files");
    options.add_options()
            ("train", "Required - Path to train data (Absolute)", cxxopts::value<std::string>())
            ("test", "Required - Path to test data (Absolute)", cxxopts::value<std::string>())
            ("out", "Required - Output path of formated data (Absolute)", cxxopts::value<std::string>())
            ("split", "Optional - How much of the data should be training data (0,1)", cxxopts::value<double>())
            ("valtrainsplit", "Optional - How much of the training data should be put into the validation set (0,1)", cxxopts::value<double>())
            ("h,help", "Print usage")
            ;
    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        exit(0);
    }
    Arguments arguments;
    if (!result.count("train")) {
        std::cout << "Missing train path." << std::endl;
        exit(1);
    } else {
        arguments.trainPath = result["train"].as<std::string>();
    }

    if (!result.count("test")) {
        std::cout << "Missing test path." << std::endl;
        exit(1);
    } else {
        arguments.testPath = result["test"].as<std::string>();
    }

    if (!result.count("out")) {
        std::cout << "Missing out path." << std::endl;
        exit(1);
    }
    else {
        arguments.outPath = result["out"].as<std::string>();
    }

    if (result.count("split")) {
        const double split = result["split"].as<double>();
        if (split < 0 || split > 1) {
            std::cout << "Split must be between 0 and 1." << std::endl;
            exit(1);
        } else if (split == 0) {
            std::cout << "Split must be over 0, as it is impossible to create features from noting." << std::endl;
            exit(1);
        } else {
            arguments.split = split;
        }
    }

    if (result.count("valtrainsplit")) {
        const double valtrainsplit = result["valtrainsplit"].as<double>();
        if (valtrainsplit < 0 || valtrainsplit > 1) {
            std::cout << "Valtrainsplit must be between 0 and 1." << std::endl;
            exit(1);
        }
        else if (valtrainsplit == 0) {
            std::cout << "Valtrainsplit must be over 0, as it is impossible to create features from noting." << std::endl;
            exit(1);
        }
        else {
            arguments.valtrainsplit = valtrainsplit;
        }
    }

    return arguments;
}

int ConvertData(Arguments arguments) {
    uint id = Logger::Begin("Reading Data");
    const auto tempTrainData = FileHanding::ReadCSV(arguments.trainPath, "\t");
    const auto tempTestData = FileHanding::ReadCSV(arguments.testPath, "\t");
    Logger::End(id);

    if (arguments.split == -1)
        arguments.split = (double)tempTrainData.size() / (double)tempTestData.size();


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
    std::vector<Feature> features = FeatureFinding::GenerateFeatureTree(3, trainData, SeriesUtils::GetCount(trainData), arguments.minWindowSize, arguments.maxWindowSize);
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
        const std::string dirPath = arguments.outPath + "/data/" + std::to_string(seriesSet.first) + "/";
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
            FileHanding::WriteFile(filePath, featureSeries);
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
    auto arguments = ParseArguments(argc, argv);
    Logger::End(id);

    return ConvertData(arguments);
}