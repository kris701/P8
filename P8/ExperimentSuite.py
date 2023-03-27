import os
import csv
import time
import configparser
import shutil

from DataConverters.DataConverterOptions import DataConverterOptions
from DataConverters import DataConverterBuilder
from NetTrainers.ProtoNetTrainer.NetOptions import NetOptions
from NetTrainers import NetTrainerBuilder
from Datasets import DatasetBuilder

class ExperimentSuite():
    ExperimentConfigDir : str = "Experiments/Configs";
    ExperimentResultsDir : str = "Experiments/Results";
    ExperimentsToRun : list;

    def __init__(self, experimentsToRun : list) -> None:
        self.ExperimentsToRun = experimentsToRun

    def RunExperiments(self):
        print("Running experiments...")
        timestamp = time.strftime("%Y%m%d-%H%M%S")
        os.makedirs(os.path.join(self.ExperimentResultsDir, timestamp))

        with open(os.path.join(self.ExperimentResultsDir, timestamp, "run " + timestamp + ".csv"), 'w', newline='') as csvfile:
            csvWriter = csv.writer(csvfile, delimiter=',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
            csvWriter.writerow(['Experiment Name', 'Best train accuracy', 'Best test accuracy'])

            for expName in self.ExperimentsToRun:
                configName = os.path.join(self.ExperimentConfigDir, expName + ".ini")

                dataLoaderOptions = DataConverterOptions()
                self._ParseConfigIntoObject(configName, "DATACONVERTER", dataLoaderOptions)
                dataLoaderOptions.VerifySettings();
                dataConverter = DataConverterBuilder.GetDataConverter(dataLoaderOptions.UseConverter)(dataLoaderOptions)
            
                print("Formatting Dataset")
                dataConverter.ConvertData()

                protonetOptions = NetOptions()
                self._ParseConfigIntoObject(configName, "NETTRAINER", protonetOptions)
                protonetOptions.VerifySettings();
                protonetOptions.experiment_root = os.path.join(self.ExperimentResultsDir, timestamp, expName)
                protonet = NetTrainerBuilder.GetNetTrainer(protonetOptions.trainer_name)(protonetOptions, DatasetBuilder.GetDataset(protonetOptions.dataset_name))

                print("Training Model")
                bestTrainAcc = protonet.Train();
                print("Best train acc: " + str(bestTrainAcc))

                print("Testing Model")
                bestTestAcc = protonet.Test();
                print("Avg test acc: " + str(bestTestAcc))

                print("Copying dataset...")
                shutil.make_archive(os.path.join(self.ExperimentResultsDir, timestamp, expName + "-dataset"), 'zip', dataLoaderOptions.FormatedFolder)

                csvWriter.writerow([expName, bestTrainAcc, bestTestAcc])

        print("Experiments finished!")

    def _ParseConfigIntoObject(self, configName, sectionName, obj) -> None:
        config = configparser.RawConfigParser()
        config.optionxform = lambda option: option
        config.read(configName)
        for index in config[sectionName]:
            if index in obj.__annotations__:
                typeName = obj.__annotations__[index].__name__;
                if typeName == "str":
                    obj.__dict__[index] = config[sectionName][index]
                elif typeName == "bool":
                    obj.__dict__[index] = config[sectionName].getboolean(index)
                elif typeName == "int":
                    obj.__dict__[index] = config[sectionName].getint(index)
                elif typeName == "float":
                    obj.__dict__[index] = config[sectionName].getfloat(index)
                else:
                    raise Exception("Invalid config type!")
            else:
                raise Warning("Warning, key '" + index + "' not in this object!")