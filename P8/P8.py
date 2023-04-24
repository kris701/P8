import multiprocessing
import os

from ExperimentSuite import ExperimentSuite

def main():
    multiprocessing.freeze_support()

    #queueItems = ["5ShotProtonetFast.ini"]
    queueItems = os.listdir("Experiments/SuiteConfigs")

    expSuite = ExperimentSuite("Experiments/SuiteConfigs")
    expSuite.RunExperimentQueue(queueItems, True);

if __name__ == '__main__':
    main()