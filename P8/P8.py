import multiprocessing
import os

from ExperimentSuite import ExperimentSuite

def main():
    multiprocessing.freeze_support()

    #queueItems = ["5ShotProtonetFast.ini"]
    queueItems = GetFilesInDir("Experiments/SuiteConfigs")

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