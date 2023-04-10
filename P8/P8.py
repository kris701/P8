import multiprocessing

from ExperimentSuite import ExperimentSuite
from ExperimentOptions import ExperimentOptions

def main():
    multiprocessing.freeze_support()
    experiments = ["ArrowHead", "BME", "CBF", "Chinatown", "ECG200", "GunPoint", "GunPointAgeSpan", "GunPointOldVersusYoung", "ItalyPowerDemand", "MoteStrain", "Plane", "SonyAIBORobotSurface1", "SonyAIBORobotSurface2", "SyntheticControl", "ToeSegmentation1", "TwoLeadECG", "UMD", "Wine"]
    #experiments = ["ArrowHead"]

    experimentOptions = ExperimentOptions();
    experimentOptions.ExperimentsToRun = experiments;
    experimentOptions.BaseConfig = "Experiments/Configs/BaseConfigs/6ShotProtonet.ini";
    experimentOptions.GenerateGraphs = True;
    experimentOptions.GenerateClassGraphs = False;
    experimentOptions.GenerateExperimentGraph = False;
    experimentOptions.GenerateSourceGraphs = True;
    experimentOptions.GenerateShapeletGraphs = False;
    experimentOptions.ComparisonData = ["../ComparisonData/6shot.csv"];
    experimentOptions.DebugMode = True;
    experimentOptions.ZipDataset = False;
    experimentOptions.ForceRemakeDataset = True;

    expSuite = ExperimentSuite(experimentOptions)
    expSuite.RunExperiments()

if __name__ == '__main__':
    main()