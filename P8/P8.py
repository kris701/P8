import multiprocessing

from ExperimentSuite import ExperimentSuite

def main():
    multiprocessing.freeze_support()

    queueItems = ["Experiments/SuiteConfigs/6ShotProtonetFull.ini"]

    expSuite = ExperimentSuite()
    expSuite.RunExperimentQueue(queueItems);

if __name__ == '__main__':
    main()