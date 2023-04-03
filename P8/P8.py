from ExperimentSuite import ExperimentSuite
from DataVisualisers.ShapeletHistogramVisualiser import ShapeletHistogramVisualiser
from DataVisualisers.ResultsVisualiser import ResultsVisualiser
from ResultsCombiners.CSVResultsCombiner import CSVResultsCombiner

#experiments = ["SwedishLeaf", "Beef", "CricketX", "ElectricDevices", "Wine"]
experiments = ["SwedishLeaf", "ElectricDevices"]

expSuite = ExperimentSuite(experiments, "Ours")
ourResults = expSuite.RunExperiments()

combiner = CSVResultsCombiner();
fullResults = combiner.Combine(
    ["../ComparisonData/5shot.csv"],
    [ourResults]
    );

fullVisualiser = ResultsVisualiser();
fullVisualiser.VisualiseAll(fullResults);

#print("Visualizing swedish leafs")
#visualizer = ShapeletHistogramVisualiser(".\\Formatted\\SwedishLeaf");
#visualizer.VisualizeAllClasses();
#visualizer.VisualizeClass(1);
