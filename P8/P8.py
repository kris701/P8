from ExperimentSuite import ExperimentSuite
from DataVisualisers.ShapeletHistogramVisualiser import ShapeletHistogramVisualiser
from DataVisualisers.ResultsVisualiser import ResultsVisualiser
from Comparers.StateOfTheArtCompare import StateOfTheArtCompare

#experiments = ["SwedishLeaf", "Beef", "CricketX", "ElectricDevices", "Wine"]
experiments = ["SwedishLeaf"]

expSuite = ExperimentSuite(experiments, "Ours")
ourResults = expSuite.RunExperiments()

fullCompare = StateOfTheArtCompare();
fullResults = fullCompare.GetFullComparisons(
    ["../ComparisonData/5shot.csv"],
    [ourResults]
    );

fullVisualiser = ResultsVisualiser();
fullVisualiser.VisualiseAll(fullResults);

#print("Visualizing swedish leafs")
#visualizer = ShapeletHistogramVisualiser(".\\Formatted\\SwedishLeaf");
#visualizer.VisualizeAllClasses();
#visualizer.VisualizeClass(1);
