import multiprocessing

from ExperimentSuite import ExperimentSuite
from ExperimentOptions import ExperimentOptions

def main():
    multiprocessing.freeze_support()

    queueItems = ["Experiments/SuiteConfigs/6ShotProtonetFast.ini"]

    expSuite = ExperimentSuite()
    expSuite.RunExperimentQueue(queueItems);

if __name__ == '__main__':
    main()