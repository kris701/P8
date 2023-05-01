import multiprocessing
import os

from ExperimentSuite import ExperimentSuite

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

    #queueItems = ["6ShotKNNFast.ini"]
    queueItems = noiseConfigs_6Shot + noiseConfigs_8Shot + smoothConfigs_6Shot + smoothConfigs_8Shot + purgeConfigs_6Shot + purgeConfigs_8Shot + featureCountConfigs_6Shot + featureCountConfigs_8Shot

    expSuite = ExperimentSuite()
    expSuite.RunExperimentQueue(queueItems, True);

def GetConfigsInDir(path : str) -> list:
    retItems = []
    for item in os.listdir(path):
        if os.path.isfile(os.path.join(path, item)):
            retItems.append(os.path.join(path, item))
    return retItems

if __name__ == '__main__':
    main()