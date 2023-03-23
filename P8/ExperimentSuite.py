import os
import csv
import time

from DataConverters.DataConverterOptions import DataConverterOptions
from DataConverters import DataConverterBuilder
from NetTrainers.ProtoNetTrainer.NetOptions import NetOptions
from NetTrainers import NetTrainerBuilder
from Datasets import DatasetBuilder

class ExperimentSuite():
    ExperimentConfigDir : str = "Experiments/Configs";
    ExperimentOutputDir : str = "Experiments/Output";
    ExperimentsToRun : list;

    def __init__(self, experimentsToRun : list) -> None:
        self.ExperimentsToRun = experimentsToRun

    def RunExperiments(self):
        print("Running experiments...")
        with open(os.path.join(self.ExperimentOutputDir, "run " + time.strftime("%Y%m%d-%H%M%S") + ".csv"), 'w', newline='') as csvfile:
            spamwriter = csv.writer(csvfile, delimiter=',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
            spamwriter.writerow(['Experiment Name', 'Best train accuracy', 'Best test accuracy'])
            for expName in self.ExperimentsToRun:
                dataLoaderOptions = DataConverterOptions(os.path.join(self.ExperimentConfigDir, expName + ".ini"))
                dataConverter = DataConverterBuilder.GetDataConverter(dataLoaderOptions.UseConverter)(dataLoaderOptions)
            
                print("Formatting Dataset")
                dataConverter.ConvertData()

                protonetOptions = NetOptions(os.path.join(self.ExperimentConfigDir, expName + ".ini"))
                protonet = NetTrainerBuilder.GetNetTrainer(protonetOptions.trainer_name)(protonetOptions, DatasetBuilder.GetDataset(protonetOptions.dataset_name))

                print("Training Model")
                bestTrainAcc = protonet.Train();

                print("Testing Model")
                bestTestAcc = protonet.Test();

                spamwriter.writerow([expName, bestTrainAcc, bestTestAcc])

