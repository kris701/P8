#include <iostream>
#include <string>
#include "cxxopts/cxxopts.hpp"
#include "src/FileHanding.h"
#include "src/FeatureFinding.h"
#include "src/SeriesUtils.h"
#include "src/Logger.h"

struct Arguments {
    std::string trainPath;
    std::string testPath;
    std::optional<double> split;
    std::optional<uint> minWindowSize;
    std::optional<uint> maxWindowSize;
};

Arguments ParseArguments (int argc, char **argv) {
    cxxopts::Options options("FeatureExtraction", "Extracts features from given files");
    options.add_options()
            ("train", "Required - Path to train data (Absolute)", cxxopts::value<std::string>())
            ("test", "Required - Path to test data (Absolute)", cxxopts::value<std::string>())
            ("split", "Optional - How much of the data should be training data (0,1)", cxxopts::value<double>())
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

    return arguments;
}

int main(int argc, char **argv) {
    uint id = Logger::Begin("Parsing Arguments");
    auto arguments = ParseArguments(argc, argv);
    Logger::End(id);

    id = Logger::Begin("Reading Data");
    const auto tempTrainData = FileHanding::ReadCSV(arguments.trainPath, "\t");
    const auto tempTestData = FileHanding::ReadCSV(arguments.testPath, "\t");
    Logger::End(id);

    id = Logger::Begin("Generating Data based on Split.");
    if (!arguments.split.has_value())
        arguments.split = (double) tempTrainData.size() / (double) tempTestData.size();

    std::vector<LabelledSeries> combinedData;
    for (const auto &seriesSet : { tempTrainData, tempTestData })
        for (const auto &s : seriesSet)
            combinedData.push_back(s);
    std::vector<LabelledSeries> trainData;
    const uint trainCount = (uint) std::round((double) combinedData.size() * arguments.split.value());
    for (uint i = 0; i < trainCount; i++)
        trainData.push_back(combinedData.at(i));
    Logger::End(id);


    id = Logger::Begin("Generating Feature Set");
    const auto featureSet = FeatureFinding::GenerateFeatureTree(3, trainData, SeriesUtils::GetCount(trainData));
    Logger::End(id);

    id = Logger::Begin("Generating Data Map");
    std::unordered_map<int, std::vector<Series>> mappedData;
    for (const auto &d : combinedData)
        mappedData[d.label].push_back(d.series);
    Logger::End(id);

    id = Logger::Begin("Writing Features to Files");
    if (std::filesystem::is_directory("out"))
        std::filesystem::remove_all("out");
    std::unordered_map<int, std::vector<std::string>> paths;
    for (const auto &seriesSet : mappedData) {
        const std::string dirPath = "out/data/" + std::to_string(seriesSet.first) + "/";
        for (uint i = 0; i < seriesSet.second.size(); i++) {
            const std::string filePath = dirPath + std::to_string(i);
            paths[seriesSet.first].push_back(filePath);
            const auto featureSeries = FeatureFinding::GenerateFeatureSeries(seriesSet.second.at(i), featureSet);
            FileHanding::WriteFile(filePath, featureSeries);
        }
    }
    Logger::End(id);
    id = Logger::Begin("Writing Split Files");
    std::vector<std::string> trainPaths;
    std::vector<std::string> testPaths;
    for (const auto &pathSet : paths) {
        const uint testIndex = (uint) (arguments.split.value() * (double) pathSet.second.size());
        for (uint i = 0; i < testIndex; i++)
            trainPaths.push_back(pathSet.second.at(i));
        for (uint i = testIndex; i < pathSet.second.size(); i++)
            testPaths.push_back(pathSet.second.at(i));
    }
    FileHanding::WriteFile("out/split/train.txt", trainPaths);
    FileHanding::WriteFile("out/split/test.txt", testPaths);
    Logger::End(id);

    return 0;
}
