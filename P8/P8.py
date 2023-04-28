import multiprocessing
import os

from ExperimentSuite import ExperimentSuite

def main():
    multiprocessing.freeze_support()

    queueItems = [
                  "6ShotProtonetFullPurge.ini",
                  "8ShotProtonetFullPurge.ini",

                  "FeatureCountTests/6ShotProtonetFull4Features.ini",
                  "FeatureCountTests/6ShotProtonetFull8Features.ini",
                  "FeatureCountTests/6ShotProtonetFull16Features.ini",
                  "FeatureCountTests/6ShotProtonetFull32Features.ini",
                  "FeatureCountTests/6ShotProtonetFull64Features.ini",
                  "FeatureCountTests/6ShotProtonetFull128Features.ini",
                  "FeatureCountTests/6ShotProtonetFull256Features.ini",

                  "FeatureCountTests/8ShotProtonetFull4Features.ini",
                  "FeatureCountTests/8ShotProtonetFull8Features.ini",
                  "FeatureCountTests/8ShotProtonetFull16Features.ini",
                  "FeatureCountTests/8ShotProtonetFull32Features.ini",
                  "FeatureCountTests/8ShotProtonetFull64Features.ini",
                  "FeatureCountTests/8ShotProtonetFull128Features.ini",
                  "FeatureCountTests/8ShotProtonetFull256Features.ini",
                  ]
    #queueItems = ["6ShotKNNFast.ini"]
    #queueItems = GetFilesInDir("Experiments/SuiteConfigs")

    expSuite = ExperimentSuite("Experiments/SuiteConfigs")
    expSuite.RunExperimentQueue(queueItems, True);

def GetFilesInDir(path : str) -> list:
    retItems = []
    for item in os.listdir(path):
        if os.path.isfile(os.path.join(path, item)):
            retItems.append(item)
    return retItems

if __name__ == '__main__':
    main()