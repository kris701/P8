import multiprocessing
import os

from ExperimentSuite import ExperimentSuite
from ResultsCombiner import ResultsCombiner

def main():
    multiprocessing.freeze_support()

    queueItems = [];

    featureCountConfigs_6Shot = GetConfigsInDir("Experiments/SuiteConfigs/FeatureCount/Protonet/6Shot");
    featureCountConfigs_8Shot = GetConfigsInDir("Experiments/SuiteConfigs/FeatureCount/Protonet/8Shot");

    noiseConfigs_6Shot = GetConfigsInDir("Experiments/SuiteConfigs/Noise/Protonet/6Shot");
    noiseConfigs_8Shot = GetConfigsInDir("Experiments/SuiteConfigs/Noise/Protonet/8Shot");

    smoothConfigs_6Shot = GetConfigsInDir("Experiments/SuiteConfigs/Smooth/Protonet/6Shot");
    smoothConfigs_8Shot = GetConfigsInDir("Experiments/SuiteConfigs/Smooth/Protonet/8Shot");
    
    purgeConfigs_6Shot = GetConfigsInDir("Experiments/SuiteConfigs/Purge/Protonet/6Shot");
    purgeConfigs_8Shot = GetConfigsInDir("Experiments/SuiteConfigs/Purge/Protonet/8Shot");

    attributeConfigs_6Shot = GetConfigsInDir("Experiments/SuiteConfigs/Attributes/Protonet/6Shot");
    attributeConfigs_8Shot = GetConfigsInDir("Experiments/SuiteConfigs/Attributes/Protonet/8Shot");

    optimalConfigs_6Shot = GetConfigsInDir("Experiments/SuiteConfigs/Optimal/Protonet/6Shot");
    optimalConfigs_8Shot = GetConfigsInDir("Experiments/SuiteConfigs/Optimal/Protonet/8Shot");

    #queueItems = ["6ShotKNNFast.ini"]
    #queueItems = noiseConfigs_6Shot + noiseConfigs_8Shot + smoothConfigs_6Shot + smoothConfigs_8Shot + purgeConfigs_6Shot + purgeConfigs_8Shot + featureCountConfigs_6Shot + featureCountConfigs_8Shot
    #queueItems = attributeConfigs_6Shot + attributeConfigs_8Shot
    queueItems = optimalConfigs_6Shot + optimalConfigs_8Shot

    expSuite = ExperimentSuite()
    expSuite.RunExperimentQueue(queueItems, True);

    #CombineAttributesCSVs()
    CombineOptimalCSVs()

def GetConfigsInDir(path : str) -> list:
    retItems = []
    for item in os.listdir(path):
        if os.path.isfile(os.path.join(path, item)):
            retItems.append(os.path.join(path, item))
    return retItems

def CombineAttributesCSVs():
    combiner = ResultsCombiner()
    combiner.CombineDatasetsIn(
        "Experiments/Results",
        ["6Shot", "Attribute"],
        [],
        "Experiments/Results/6shot_Features.csv");

    combiner.CombineDatasetsIn(
        "Experiments/Results",
        ["8Shot", "Attribute"],
        [],
        "Experiments/Results/8shot_Features.csv");

def CombineFeatureCountCSVs():
    combiner = ResultsCombiner()
    combiner.CombineDatasetsIn(
        "Experiments/Results",
        ["6Shot", "Features"],
        ["Purge", "Freq", "MaxDist", "MinDist", "OccPoss"],
        "Experiments/Results/6shot_Features.csv");

    combiner.CombineDatasetsIn(
        "Experiments/Results",
        ["8Shot", "Features"],
        ["Purge", "Freq", "MaxDist", "MinDist", "OccPoss"],
        "Experiments/Results/8shot_Features.csv");

def CombineSmoothCSVs():
    combiner = ResultsCombiner()
    combiner.CombineDatasetsIn(
        "Experiments/Results",
        ["6Shot", "Smooth"],
        [],
        "Experiments/Results/6shot_Smooth.csv");
    combiner.CombineDatasetsIn(
        "Experiments/Results",
        ["8Shot", "Smooth"],
        [],
        "Experiments/Results/8shot_Smooth.csv");

def CombineNoiseCSVs():
    combiner = ResultsCombiner()
    combiner.CombineDatasetsIn(
        "Experiments/Results",
        ["6Shot", "Noise"],
        [],
        "Experiments/Results/6shot_Noise.csv");
    combiner.CombineDatasetsIn(
        "Experiments/Results",
        ["8Shot", "Noise"],
        [],
        "Experiments/Results/8shot_Noise.csv");

def CombinePurgeCSVs():
    combiner = ResultsCombiner()
    combiner.CombineDatasetsIn(
        "Experiments/Results",
        ["6Shot", "Purge"],
        [],
        "Experiments/Results/6shot_Purge.csv");
    combiner.CombineDatasetsIn(
        "Experiments/Results",
        ["8Shot", "Purge"],
        [],
        "Experiments/Results/8shot_Purge.csv");

def CombineOptimalCSVs():
    combiner = ResultsCombiner()
    combiner.CombineDatasetsIn(
        "Experiments/Results",
        ["6Shot", "Optimal"],
        [],
        "Experiments/Results/6shot_Optimal.csv");
    combiner.CombineDatasetsIn(
        "Experiments/Results",
        ["8Shot", "Optimal"],
        [],
        "Experiments/Results/8shot_Optimal.csv");
    
if __name__ == '__main__':
    main()