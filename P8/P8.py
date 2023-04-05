import multiprocessing

from ExperimentSuite import ExperimentSuite

def main():
    multiprocessing.freeze_support()
    experiments = ["ArrowHead", "BME", "CBF", "Chinatown", "ECG200", "GunPoint", "GunPointAgeSpan", "GunPointOldVersusYoung", "ItalyPowerDemand", "MoteStrain", "Plane", "SonyAIBORobotSurface1", "SonyAIBORobotSurface2", "SyntheticControl", "ToeSegmentation1", "TwoLeadECG", "UMD", "Wine"]
    #experiments = ["ArrowHead"]

    expSuite = ExperimentSuite(experiments, "Experiments/Configs/Base.ini", "Ours", True, True)
    expSuite.RunExperiments(True)

if __name__ == '__main__':
    main()