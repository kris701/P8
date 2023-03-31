from ExperimentSuite import ExperimentSuite
from DataVisualisers.ShapeletHistogramVisualiser import ShapeletHistogramVisualiser

#experiments = ["SwedishLeaf", "Beef", "CricketX", "ElectricDevices", "Wine"]
experiments = ["SwedishLeaf"]

expSuite = ExperimentSuite(experiments)
expSuite.RunExperiments()

print("Visualizing swedish leafs")
visualizer = ShapeletHistogramVisualiser(".\\Formatted\\SwedishLeaf");
visualizer.VisualizeAllClasses();
visualizer.VisualizeClass(1);
