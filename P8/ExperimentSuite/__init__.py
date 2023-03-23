import os

class ExperimentSuite():
    ExperimentConfigDir : str = "Experiments/Configs";
    ExperimentOutputDir : str = "Experiments/Output";

    def __init__(self, experimentConfigDir : str, experimentOutputDir : str) -> None:
        self.ExperimentConfigDir = experimentConfigDir;
        self.ExperimentOutputDir = experimentOutputDir;

    def RunExperiments(self):
        expDirs = os.walk(self.ExperimentConfigDir)

        for expDir in expDirs:
            pass