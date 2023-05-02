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

    queueItems = noiseConfigs_6Shot + noiseConfigs_8Shot + smoothConfigs_6Shot + smoothConfigs_8Shot + purgeConfigs_6Shot + purgeConfigs_8Shot + featureCountConfigs_6Shot + featureCountConfigs_8Shot
    queueItems = ["Experiments/SuiteConfigs/6ShotKNNFast.ini"]

    expSuite = ExperimentSuite()
    #expSuite.RunExperimentQueue(queueItems, True);

    # Combine all results into one file
    combiner = ResultsCombiner()
    combiner.CombineDatasetsIn(
        "Experiments/Results",
        "6Shot",
        "Experiments/Results/6shotFull.csv");
    combiner.CombineDatasetsIn(
        "Experiments/Results",
        "8Shot",
        "Experiments/Results/8shotFull.csv");

def GetConfigsInDir(path : str) -> list:
    retItems = []
    for item in os.listdir(path):
        if os.path.isfile(os.path.join(path, item)):
            retItems.append(os.path.join(path, item))
    return retItems

if __name__ == '__main__':
    main()