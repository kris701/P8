import multiprocessing
import os

from ExperimentSuite import ExperimentSuite

def main():
    multiprocessing.freeze_support()

    queueItems = ["6ShotProtonetFull4Features.ini",
                  "6ShotProtonetFull8Features.ini",
                  "6ShotProtonetFull16Features.ini",
                  "6ShotProtonetFull32Features.ini",
                  "6ShotProtonetFull.ini",
                  "6ShotProtonetFull128Features.ini",
                  "6ShotProtonetFull256Features.ini",

                  "8ShotProtonetFull4Features.ini",
                  "8ShotProtonetFull8Features.ini",
                  "8ShotProtonetFull16Features.ini",
                  "8ShotProtonetFull32Features.ini",
                  "8ShotProtonetFull.ini",
                  "8ShotProtonetFull128Features.ini",
                  "8ShotProtonetFull256Features.ini",
                  ]
    #queueItems = ["5ShotProtonetFast.ini"]
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