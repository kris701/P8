#include <string>
#include "src/IO/FileHanding.h"
#include "core/feature_finding/FeatureFinding.h"
#include "src/IO/Logger.h"
#include "src/IO/ArgumentParser.h"
#include "core/attributes/AttributeBuilder.h"
#include "src/preprocessing/DataAugmentation.h"
#include "src/preprocessing/DataPurge.h"
#include "src/preprocessing/DataSplit.h"
#include "types/FeatureHistogramBuilder.h"
#include "types/SeriesMap.h"

int main(int argc, char** argv) {
    uint id = Logger::Begin("Parsing Arguments");
    auto arguments = ArgumentParsing::ParseArguments(argc, argv);
    Logger::End(id);

    id = Logger::Begin("Reading Data");
    auto data = FileHanding::ReadCSV({ arguments.trainPath, arguments.testPath }, "\t");
    Logger::End(id);

    id = Logger::Begin("preprocessing Data");
    auto id2 = Logger::Begin("Normalizing");
    data.MinMaxNormalize();
    data.ForcePositiveRange();
    Logger::End(id2);

    SeriesMap candidates = data;
    SeriesMap rejects;

    if (arguments.purge) {
        id2 = Logger::Begin("Purging");
        const auto purgeResult = DataPurge::Purge(data);
        candidates = purgeResult.acceptable;
        rejects = purgeResult.rejects;
        Logger::End(id2);
        id2 = Logger::Begin("Writing Purged to Files");
        const auto purgePath = arguments.outPath + "purged/";
        FileHanding::WriteToFiles(purgePath + "candidates/", SeriesMap(candidates));
        FileHanding::WriteToFiles(purgePath + "rejects/", SeriesMap(rejects));
        Logger::End(id2);
    }

    id2 = Logger::Begin("Splitting");
    const auto map = SeriesMap(candidates);
    const auto splitData = DataSplit::Split(map, arguments.split);
    auto testData = splitData.test;
    testData.InsertAll(rejects);
    Logger::End(id2);

    id2 = Logger::Begin("Writing Source Train Files");
    const auto sourceTrainPath = arguments.outPath + "source/";
    FileHanding::WriteToFiles(sourceTrainPath + "original/", SeriesMap(splitData.train));
    FileHanding::WriteToFiles(sourceTrainPath + "augmentation/", SeriesMap(DataAugmentation::Augment(splitData.train,arguments.deleteOriginal,arguments.smoothingDegree,arguments.noisifyAmount)));
    Logger::End(id2);

    id2 = Logger::Begin("Augmenting Data");
    const auto trainData =
            DataAugmentation::Augment(splitData.train, arguments.deleteOriginal, arguments.smoothingDegree, arguments.noisifyAmount);
    const auto trainMap = SeriesMap(trainData);
    const auto testMap = SeriesMap(testData);
    Logger::End(id2);
    Logger::End(id);

    id = Logger::Begin("Generating Feature Set");
    auto features = FeatureFinding::GenerateFeaturesFromSamples(
            trainMap, arguments.minWindowSize, arguments.maxWindowSize,
            arguments.minSampleSize, arguments.maxSampleSize, arguments.featureCount,
            AttributeBuilder::GenerateAttributes(arguments.attributes));
    Logger::End(id);

    id = Logger::Begin("Generating Feature Points");
    const auto trainHistograms = FeatureHistogramBuilder::BuildSet(trainData, features);
    const auto testHistograms = FeatureHistogramBuilder::BuildSet(trainData, features);
    Logger::End(id);

    id = Logger::Begin("Writing Feature Series to Files");
    const auto featureSeriesPath = arguments.outPath + "data/";
    std::vector<std::string> trainFiles = FileHanding::WriteToFiles(featureSeriesPath, trainHistograms);
    std::vector<std::string> testFiles = FileHanding::WriteToFiles(featureSeriesPath, testHistograms);
    Logger::End(id);

    id = Logger::Begin("Shuffles File Order");
    std::shuffle(std::begin(trainFiles), std::end(trainFiles), g);
    std::shuffle(std::begin(testFiles), std::end(testFiles), g);
    Logger::End(id);

    id = Logger::Begin("Writing Split Files");
    const auto splitPath = arguments.outPath + "split/";
    FileHanding::WriteFile(splitPath + "train.txt", FileHanding::RemoveSubPath(arguments.outPath, trainFiles));
    FileHanding::WriteFile(splitPath + "test.txt", FileHanding::RemoveSubPath(arguments.outPath, testFiles));
    Logger::End(id);

    id = Logger::Begin("Writing Feature Files");
    const auto featurePath = arguments.outPath + "features/";
    const auto shapeletPath = featurePath + "shapelets/";
    const auto shapeletFiles = FileHanding::RemoveSubPath(featurePath, FileHanding::WriteToFiles(shapeletPath, features.Shapelets()));
    FileHanding::WriteCSV(featurePath + "features.csv",
                          Feature::Header(),
                          features.FeatureCSV(shapeletFiles));
    Logger::End(id);

    return 0;
}