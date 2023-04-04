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
        const uint featureCount;
        const uint sampleSize;
        Arguments(const std::string &trainPath, const std::string &testPath, const std::string outPath, double split, double valTrainSplit,
                  uint minWindowSize, uint maxWindowSize, uint featureCount, uint sampleSize) :
                  trainPath(trainPath), testPath(testPath), outPath(outPath), split(split), valtrainsplit(valTrainSplit),
                  minWindowSize(minWindowSize), maxWindowSize(maxWindowSize), featureCount(featureCount), sampleSize(sampleSize) {}
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
                ("maxWindowSize", "Maximum size of windows. 0 for max possible, same if larger than series length.", cxxopts::value<uint>() -> default_value("32"))
                ("featureCount", "How many features to generate", cxxopts::value<uint>() -> default_value("32"))
                ("sampleSize", "How many samples of each class is used to generate a single feature", cxxopts::value<uint>() -> default_value("3"))
                ("h,help", "Print usage")
                ;
        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            exit(0);
        }

        try {
            return Arguments(
                    result["train"].as<std::string>(),
                    result["test"].as<std::string>(),
                    result["out"].as<std::string>(),
                    result["split"].as<double>(),
                    result["valtrainsplit"].as<double>(),
                    result["minWindowSize"].as<uint>(),
                    result["maxWindowSize"].as<uint>(),
                    result["featureCount"].as<uint>(),
                    result["sampleSize"].as<uint>()
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
