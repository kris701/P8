#ifndef FEATUREEXTRACTION_ARGUMENTPARSER_H
#define FEATUREEXTRACTION_ARGUMENTPARSER_H

#include <string>
#include <iostream>
#include <cxxopts/cxxopts.hpp>

namespace ArgumentParsing {
    struct Arguments {
        const std::string dataPath;
        const int nNeighbors;
        Arguments(const std::string dataPath, const int nNeighbors) : dataPath(dataPath), nNeighbors(nNeighbors) {}
    };

    static Arguments ParseArguments(int argc, char **argv) {
        cxxopts::Options options("FeatureExtraction", "Extracts features from given files");
        options.add_options()
                ("data", "Path to data (Absolute)", cxxopts::value<std::string>())
                ("neighbors", "How many nearest neighbors to consider", cxxopts::value<int>()->default_value("3"))
                ("h,help", "Print usage")
                ;
        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            exit(0);
        }

        try {
            return Arguments(
                    result["data"].as<std::string>(),
                    result["neighbors"].as<int>()
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
