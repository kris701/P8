import multiprocessing

from ExperimentSuite import ExperimentSuite

def main():
    multiprocessing.freeze_support()

    queueItems = ["6ShotProtonetFast.ini"]

    expSuite = ExperimentSuite("Experiments/SuiteConfigs")
    expSuite.RunExperimentQueue(queueItems, True);

if __name__ == '__main__':
    main()