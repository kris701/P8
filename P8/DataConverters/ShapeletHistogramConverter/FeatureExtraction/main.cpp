#include <iostream>
#include <string>
#include "src/IO/FileHanding.h"
#include "core/feature_finding/FeatureFinding.h"
#include "src/utilities/SeriesUtils.h"
#include "src/IO/Logger.h"
#include "src/IO/ArgumentParser.h"
#include "src/utilities/FeatureUtils.h"
#include "core/attributes/AttributeBuilder.h"
#include "core/DataAugmentation.h"

int main(int argc, char** argv) {
    uint id = Logger::Begin("Parsing Arguments");
    auto arguments = ArgumentParsing::ParseArguments(argc, argv);
    Logger::End(id);

    id = Logger::Begin("Reading Data");
    auto data = FileHanding::ReadCSV({ arguments.trainPath, arguments.testPath }, "\t");
    Logger::End(id);

    id = Logger::Begin("Normalizing Data");
    SeriesUtils::MinMaxNormalize(data);
    SeriesUtils::ForcePositiveRange(data);
    Logger::End(id);

    id = Logger::Begin("Shuffling Data");
    std::shuffle(data.begin(), data.end(), g);
    Logger::End(id);

    id = Logger::Begin("Splitting Data");
    const auto valSplit = SeriesUtils::Split(data, arguments.valtrainsplit);
    const auto valData = valSplit.first;
    const auto trainSplit = SeriesUtils::Split(valSplit.second, arguments.split);
    auto trainData = trainSplit.first;

    uint id2 = Logger::Begin("Augmenting Data");
    trainData = DataAugmentation::Augment(trainData, false, arguments.smoothingDegree, arguments.noisifyAmount);
    Logger::End(id2);

    const auto testData = trainSplit.second;
    const auto trainMap = SeriesUtils::ToMap(trainData);
    const auto testMap = SeriesUtils::ToMap(testData);
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
    const auto valFeatures = FeatureUtils::GenerateFeatureSeries(valData, features);
    Logger::End(id);

    id = Logger::Begin("Writing Feature Series to Files");
    const auto featureSeriesPath = arguments.outPath + "data/";
    std::vector<std::string> trainFiles = FileHanding::WriteToFiles(featureSeriesPath, trainFeatures);
    std::vector<std::string> testFiles = FileHanding::WriteToFiles(featureSeriesPath, testFeatures);
    std::vector<std::string> valFiles = FileHanding::WriteToFiles(featureSeriesPath, valFeatures);
    Logger::End(id);

    id = Logger::Begin("Shuffles File Order");
    std::shuffle(std::begin(trainFiles), std::end(trainFiles), g);
    std::shuffle(std::begin(testFiles), std::end(testFiles), g);
    std::shuffle(std::begin(valFiles), std::end(valFiles), g);
    Logger::End(id);

    id = Logger::Begin("Writing Split Files");
    const auto splitPath = arguments.outPath + "split/";
    FileHanding::WriteFile(splitPath + "train.txt", FileHanding::RemoveSubPath(arguments.outPath, trainFiles));
    FileHanding::WriteFile(splitPath + "test.txt", FileHanding::RemoveSubPath(arguments.outPath, testFiles));
    FileHanding::WriteFile(splitPath + "val.txt", FileHanding::RemoveSubPath(arguments.outPath, valFiles));
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