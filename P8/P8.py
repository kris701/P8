from ExperimentSuite import ExperimentSuite

experiments = ["SwedishLeaf"]

expSuite = ExperimentSuite(experiments)
result = expSuite.RunExperiments()

print(result)