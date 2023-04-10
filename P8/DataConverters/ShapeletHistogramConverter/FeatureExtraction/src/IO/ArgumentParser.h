#ifndef FEATUREEXTRACTION_ARGUMENTPARSER_H
#define FEATUREEXTRACTION_ARGUMENTPARSER_H

#include <string>
#include <iostream>
#include "../../include/cxxopts/cxxopts.hpp"

namespace ArgumentParsing {
    struct Arguments {
        const std::string trainPath;
        const std::string testPath;
        const std::string outPath;
        const double split;
        const double valtrainsplit;
        const uint minWindowSize;
        const uint maxWindowSize;
        const uint minSampleSize;
        const uint maxSampleSize;
        const uint featureCount;
        const std::vector<std::string> attributes;
        Arguments(const std::string &trainPath, const std::string &testPath, const std::string outPath, double split, double valTrainSplit,
                  uint minWindowSize, uint maxWindowSize, uint minSampleSize, uint maxSampleSize, uint featureCount,
                  std::vector<std::string> attributes) :
                  trainPath(trainPath), testPath(testPath), outPath(outPath), split(split), valtrainsplit(valTrainSplit),
                  minWindowSize(minWindowSize), maxWindowSize(maxWindowSize),
                  minSampleSize(minSampleSize), maxSampleSize(maxSampleSize), featureCount(featureCount),
                  attributes(attributes){}
    };

    Arguments ParseArguments(int argc, char **argv) {
        cxxopts::Options options("FeatureExtraction", "Extracts features from given files");
        options.add_options()
                ("train", "Path to train data (Absolute)", cxxopts::value<std::string>())
                ("test", "Path to test data (Absolute)", cxxopts::value<std::string>())
                ("out", "Output path of formated data (Absolute)", cxxopts::value<std::string>())
                ("split", "How much of the data should be training data. [0.0,1.0) for percent, [1, n) for data points", cxxopts::value<double>() -> default_value("5"))
                ("valtrainsplit", "How much of the data should be put into the validation set (0,1)", cxxopts::value<double>() -> default_value("0"))
                ("minWindowSize", "Minimum size of windows. Should be between 2 and maxWindowSize.", cxxopts::value<uint>() -> default_value("2"))
                ("maxWindowSize", "Maximum size of windows. 0 for max possible, same if larger than series length.", cxxopts::value<uint>() -> default_value("64"))
                ("minSampleSize", "Minimum number of samples for each class in a given feature.", cxxopts::value<uint>() -> default_value("0"))
                ("maxSampleSize", "Maximum number of samples for each class in a given feature. 0 for maximum possible", cxxopts::value<uint>() -> default_value("5"))
                ("featureCount", "How many features to generate", cxxopts::value<uint>() -> default_value("128"))
                ("attributes", "A given attribute", cxxopts::value<std::vector<std::string>>() ->default_value("minDist"))
                ("h,help", "Print usage")
                ;
        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            exit(0);
        }

        printf("Attribute count: %zu\n", result.count("attributes"));

        try {
            return Arguments(
                    result["train"].as<std::string>(),
                    result["test"].as<std::string>(),
                    result["out"].as<std::string>(),
                    result["split"].as<double>(),
                    result["valtrainsplit"].as<double>(),
                    result["minWindowSize"].as<uint>(),
                    result["maxWindowSize"].as<uint>(),
                    result["minSampleSize"].as<uint>(),
                    result["maxSampleSize"].as<uint>(),
                    result["featureCount"].as<uint>(),
                    result["attributes"].as<std::vector<std::string>>()
                    );
        } catch (const cxxopts::exceptions::option_has_no_value& e) {
            printf("\nMissing argument: %s\n", e.what());
            printf("Run program with -h or --help to see required arguments.\n");
            printf("Specifically those without a default value\n");
            exit(1);
        }
    }
}

#endif //FEATUREEXTRACTION_ARGUMENTPARSER_H
