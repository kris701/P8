import multiprocessing

from ExperimentSuite import ExperimentSuite

def main():
    multiprocessing.freeze_support()

    queueItems = ["Experiments/SuiteConfigs/6ShotProtonetFast.ini"]

    expSuite = ExperimentSuite()
    expSuite.RunExperimentQueue(queueItems, True);

if __name__ == '__main__':
    main()