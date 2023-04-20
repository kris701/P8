#include <iostream>
#include <string>
#include "src/IO/FileHanding.h"
#include "core/feature_finding/FeatureFinding.h"
#include "src/utilities/SeriesUtils.h"
#include "src/IO/Logger.h"
#include "src/IO/ArgumentParser.h"
#include "src/utilities/FeatureUtils.h"
#include "core/attributes/AttributeBuilder.h"
#include "Preprocessing/DataAugmentation.h"
#include "Preprocessing/DataPurge.h"
#include "Preprocessing/DataSplit.h"

int main(int argc, char** argv) {
    uint id = Logger::Begin("Parsing Arguments");
    auto arguments = ArgumentParsing::ParseArguments(argc, argv);
    Logger::End(id);

    id = Logger::Begin("Reading Data");
    auto data = FileHanding::ReadCSV({ arguments.trainPath, arguments.testPath }, "\t");
    Logger::End(id);

    id = Logger::Begin("Preprocessing Data");
    auto id2 = Logger::Begin("Normalizing");
    SeriesUtils::MinMaxNormalize(data);
    SeriesUtils::ForcePositiveRange(data);
    Logger::End(id2);

    id2 = Logger::Begin("Purging");
    const auto purgeResult = DataPurge::Purge(data);
    Logger::End(id2);

    id2 = Logger::Begin("Splitting");
    const auto map = SeriesUtils::ToMap(purgeResult.acceptable);
    const auto splitData = DataSplit::Split(map, arguments.split);
    auto testData = splitData.test;
    testData.insert(testData.end(), purgeResult.rejects.begin(), purgeResult.rejects.end());
    Logger::End(id2);

    id2 = Logger::Begin("Augmenting Data");
    const auto trainData =
            DataAugmentation::Augment(splitData.train, false, arguments.smoothingDegree, arguments.noisifyAmount);
    const auto trainMap = SeriesUtils::ToMap(trainData);
    const auto testMap = SeriesUtils::ToMap(testData);
    Logger::End(id2);
    Logger::End(id);

    id = Logger::Begin("Generating Feature Set");
    auto features = FeatureFinding::GenerateFeaturesFromSamples(
            trainMap, arguments.minWindowSize, arguments.maxWindowSize,
            arguments.minSampleSize, arguments.maxSampleSize, arguments.featureCount,
            AttributeBuilder::GenerateAttributes(arguments.attributes));
    Logger::End(id);

    id = Logger::Begin("Generating Feature Points");
    const auto trainFeatures = FeatureUtils::GenerateFeatureSeries(trainData, features);
    const auto testFeatures = FeatureUtils::GenerateFeatureSeries(testData, features);
    Logger::End(id);

    id = Logger::Begin("Writing Feature Series to Files");
    const auto featureSeriesPath = arguments.outPath + "data/";
    std::vector<std::string> trainFiles = FileHanding::WriteToFiles(featureSeriesPath, trainFeatures);
    std::vector<std::string> testFiles = FileHanding::WriteToFiles(featureSeriesPath, testFeatures);
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
    const auto shapelets = FeatureUtils::RetrieveShapelets(features);
    const auto shapeletFiles = FileHanding::RemoveSubPath(featurePath, FileHanding::WriteToFiles(shapeletPath, shapelets));
    FileHanding::WriteCSV(featurePath + "features.csv",
                          FeatureUtils::FeatureHeader(),
                          FeatureUtils::FeatureCSV(features, shapeletFiles));
    Logger::End(id);

    id = Logger::Begin("Writing Source Train Files");
    const auto sourceTrainPath = arguments.outPath + "source/";
    FileHanding::WriteToFiles(sourceTrainPath, trainMap);
    Logger::End(id);

    return 0;
}