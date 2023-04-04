from ExperimentSuite import ExperimentSuite
from DataVisualisers.ShapeletHistogramVisualiser import ShapeletHistogramVisualiser
from DataVisualisers.ResultsVisualiser import ResultsVisualiser
from ResultsCombiners.CSVResultsCombiner import CSVResultsCombiner

experiments = ["ArrowHead", "BME", "CBF3", "Chinatown", "ECG200", "GunPoint", "GunPointAgeSpan", "GunPointOldVersusYoung", "ItalyPowerDemand", "MoteStrain", "Plane", "SonyAIBORobotSurface1", "SonyAIBORobotSurface2", "SyntheticControl", "ToeSegmentation1", "TwoLeadECG", "UMD", "Wine"]

expSuite = ExperimentSuite(experiments, "Experiments/Configs/Base.ini", "Ours")
ourResults = expSuite.RunExperiments()

combiner = CSVResultsCombiner();
fullResults = combiner.Combine(
    ["../ComparisonData/6shot.csv"],
    [ourResults],
    True
    );

fullVisualiser = ResultsVisualiser();
fullVisualiser.VisualiseAll(fullResults);

#print("Visualizing swedish leafs")
#visualizer = ShapeletHistogramVisualiser(".\\Formatted\\SwedishLeaf");
#visualizer.VisualizeAllClasses();
#visualizer.VisualizeClass(1);
