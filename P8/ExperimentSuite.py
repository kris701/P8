import os
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

    def RunExperiments(self) -> dict:
        results = {}
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

            results[expName] = (bestTrainAcc, bestTestAcc)
        return results
