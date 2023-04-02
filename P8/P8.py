from ExperimentSuite import ExperimentSuite
from DataVisualisers.ShapeletHistogramVisualiser import ShapeletHistogramVisualiser

#experiments = ["SwedishLeaf", "Beef", "CricketX", "ElectricDevices", "Wine"]
experiments = ["Adiac", "Beef", "BeetleFly", "BirdChicken", "Coffee", "CricketX", "ECH200", "ElectricDevices", "FaceAll",
               "FaceFour", "FordA", "FordB", "MedicalImages", "StrawBerry", "SwedishLeaf", "SyntheticControl", "TwoPatterns",
               "Wine", "Yoga"]
#experiments = ["SwedishLeaf"]

expSuite = ExperimentSuite(experiments)
expSuite.RunExperiments()

#print("Visualizing swedish leafs")
#visualizer = ShapeletHistogramVisualiser(".\\Formatted\\SwedishLeaf");
#visualizer.VisualizeAllClasses();
#visualizer.VisualizeClass(1);
