#ifndef FEATUREEXTRACTION_ARGUMENTPARSER_H
#define FEATUREEXTRACTION_ARGUMENTPARSER_H

#include <string>
#include <iostream>
#include "cxxopts/cxxopts.hpp"

namespace ArgumentParsing {
    struct Arguments {
        const std::string trainPath;
        const std::string testPath;
        const std::string outPath;
        const double split;
        const double valtrainsplit;
        const uint minWindowSize;
        const uint maxWindowSize;
        const uint depth;
        Arguments(const std::string &trainPath, const std::string &testPath, const std::string outPath, double split, double valTrainSplit,
                  uint minWindowSize, uint maxWindowSize, uint depth) :
                  trainPath(trainPath), testPath(testPath), outPath(outPath), split(split), valtrainsplit(valTrainSplit),
                  minWindowSize(minWindowSize), maxWindowSize(maxWindowSize), depth(depth) {}
    };

    Arguments ParseArguments(int argc, char **argv) {
        cxxopts::Options options("FeatureExtraction", "Extracts features from given files");
        options.add_options()
                ("train", "Path to train data (Absolute)", cxxopts::value<std::string>())
                ("test", "Path to test data (Absolute)", cxxopts::value<std::string>())
                ("out", "Output path of formated data (Absolute)", cxxopts::value<std::string>())
                ("split", "How much of the data should be training data. [0.0,1.0) for percent, [1, n) for data points", cxxopts::value<double>() -> default_value("5"))
                ("valtrainsplit", "How much of the data should be put into the validation set (0,1)", cxxopts::value<double>() -> default_value("0"))
                ("depth", "Desc", cxxopts::value<uint>() -> default_value("40"))
                ("minWindowSize", "Desc", cxxopts::value<uint>() -> default_value("2"))
                ("maxWindowSize", "Desc", cxxopts::value<uint>() -> default_value("32"))
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
                    result["depth"].as<uint>()
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
