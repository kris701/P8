from ExperimentSuite import ExperimentSuite
from DataVisualisers.ShapeletHistogramVisualiser import ShapeletHistogramVisualiser

#experiments = ["SwedishLeaf", "Beef", "CricketX", "ElectricDevices", "Wine"]
experiments = ["SwedishLeaf"]

expSuite = ExperimentSuite(experiments)
#expSuite.RunExperiments()

visualizer = ShapeletHistogramVisualiser(".\\Formatted\\SwedishLeaf");
#visualizer.Visualize();
visualizer.VisualizeClass(1);
