#include <iostream>
#include <string>
#include <filesystem>

#include "ArgumentParsing.h"
#include "FileReading.h"
#include "Classification.h"

int main(int argc, char** argv) {
    const auto args = ArgumentParsing::ParseArguments(argc, argv);

    const auto trainPaths = FileReading::ReadSplitFile(args.dataPath + "/split/train.txt");
    const auto testPaths = FileReading::ReadSplitFile(args.dataPath + "/split/test.txt");

    const auto trainSeries = FileReading::ReadFeatureSeries(trainPaths, args.dataPath);
    const auto testSeries = FileReading::ReadFeatureSeries(testPaths, args.dataPath);

    uint correct = 0;
    uint incorrect = 0;
    for (const auto &seriesSet : testSeries)
        for (const auto &series : seriesSet.second) {
            const auto guess = Classification::KNN::Classify(trainSeries, series, args.nNeighbors);
            if (guess == seriesSet.first)
                correct++;
            else
                incorrect++;
        }

    const double accuracy = (double) correct / (double) (correct + incorrect);

    return accuracy * 100;
}