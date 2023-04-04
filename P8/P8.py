from ExperimentSuite import ExperimentSuite

#experiments = ["ArrowHead", "BME", "CBF", "Chinatown", "ECG200", "GunPoint", "GunPointAgeSpan", "GunPointOldVersusYoung", "ItalyPowerDemand", "MoteStrain", "Plane", "SonyAIBORobotSurface1", "SonyAIBORobotSurface2", "SyntheticControl", "ToeSegmentation1", "TwoLeadECG", "UMD", "Wine"]
experiments = ["ArrowHead"]

expSuite = ExperimentSuite(experiments, "Experiments/Configs/Base.ini", "Ours", True)
ourResults = expSuite.RunExperiments()
