from ExperimentSuite import ExperimentSuite
from DataVisualisers.ShapeletHistogramVisualiser import ShapeletHistogramVisualiser
from DataVisualisers.ResultsVisualiser import ResultsVisualiser
from ResultsCombiners.CSVResultsCombiner import CSVResultsCombiner

experiments = ["Adiac", "Beef", "BeetleFly", "BirdChicken", "Coffee", "CricketX", "ECH200", "ElectricDevices", "FaceAll",
               "FaceFour", "FordA", "FordB", "MedicalImages", "StrawBerry", "SwedishLeaf", "SyntheticControl", "TwoPatterns",
               "Wine", "Yoga"]
#experiments = ["SwedishLeaf", "Beef", "CricketX", "ElectricDevices", "Wine"]
#experiments = ["SwedishLeaf"]

expSuite = ExperimentSuite(experiments, "Ours")
ourResults = expSuite.RunExperiments()

combiner = CSVResultsCombiner();
fullResults = combiner.Combine(
    ["../ComparisonData/5shot.csv"],
    [ourResults],
    True
    );

fullVisualiser = ResultsVisualiser();
fullVisualiser.VisualiseAll(fullResults);

#print("Visualizing swedish leafs")
#visualizer = ShapeletHistogramVisualiser(".\\Formatted\\SwedishLeaf");
#visualizer.VisualizeAllClasses();
#visualizer.VisualizeClass(1);
