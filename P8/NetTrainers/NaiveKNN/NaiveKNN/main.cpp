#include <iostream>
#include <string>
#include <filesystem>

#include "ArgumentParsing.h"
#include "FileReading.h"
#include "Classification.h"

int main(int argc, char** argv) {
    const auto args = ArgumentParsing::ParseArguments(argc, argv);

    // For each extracted data set
    for (const auto & entry : std::filesystem::directory_iterator(args.dataPath)) {
        const auto &path = entry.path();
        const auto pathString = path.string();

        const auto trainPaths = FileReading::ReadSplitFile(pathString + "/split/train.txt");
        const auto testPaths = FileReading::ReadSplitFile(pathString + "/split/test.txt");

        const auto trainSeries = FileReading::ReadFeatureSeries(trainPaths, pathString);
        const auto testSeries = FileReading::ReadFeatureSeries(testPaths, pathString);

        uint correct = 0;
        uint incorrect = 0;
        for (const auto &seriesSet : testSeries)
            for (const auto &series : seriesSet.second) {
                const auto guess = Classification::KNN::Classify(trainSeries, series, 3);
                if (guess == seriesSet.first)
                    correct++;
                else
                    incorrect++;
            }

        const double accuracy = (double) correct / (double) (correct + incorrect);

        printf("%s: %f%%\n", path.filename().string().c_str(), accuracy);
    }
    return 0;
}