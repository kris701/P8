import os
import csv
import time
import configparser
import shutil

from DataConverters.DataConverterOptions import DataConverterOptions
from DataConverters import DataConverterBuilder
from NetTrainers.NetOptions import NetOptions
from NetTrainers import NetTrainerBuilder
from Datasets import DatasetBuilder
from DataVisualisers.ShapeletHistogramVisualiser import ShapeletHistogramVisualiser
from DataVisualisers.ResultsVisualiser import ResultsVisualiser
from ResultsCombiners.CSVResultsCombiner import CSVResultsCombiner

class ExperimentSuite():
    ExperimentConfigDir : str = "Experiments/Configs";
    ExperimentResultsDir : str = "Experiments/Results";
    ExperimentsToRun : list;
    ExperimentName : str = "Ours"
    BaseConfig : str = "Experiments/Configs/Base.ini"
    GenerateGraphs : bool = False
    GenerateClassGraphs : bool = False

    def __init__(self, experimentsToRun : list, baseConfig : str, experimentName : str, generateGraphs : bool, generateClassGraphs : bool) -> None:
        self.ExperimentsToRun = experimentsToRun
        self.ExperimentName = experimentName
        self.BaseConfig = baseConfig
        self.GenerateGraphs = generateGraphs
        self.GenerateClassGraphs = generateClassGraphs

    def RunExperiments(self) -> dict:
        print("Running experiments...")
        results = {};
        timestamp = time.strftime("%Y%m%d-%H%M%S")
        os.makedirs(os.path.join(self.ExperimentResultsDir, timestamp))

        with open(os.path.join(self.ExperimentResultsDir, timestamp, "run " + timestamp + ".csv"), 'w', newline='') as csvfile:
            csvWriter = csv.writer(csvfile, delimiter=',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
            csvWriter.writerow(['Experiment Name', 'Feature Extractor', 'Net Trainer', 'Best train accuracy', 'Best test accuracy'])

            counter : int = 1;
            for expName in self.ExperimentsToRun:
                print("   === " + expName + " started (" + str(counter) + "/" + str(len(self.ExperimentsToRun)) + ") ===   ")

                configName = os.path.join(self.ExperimentConfigDir, expName + ".ini")

                dataLoaderOptions = DataConverterOptions()
                self._ParseConfigIntoObject(self.BaseConfig, "DATACONVERTER", dataLoaderOptions)
                self._ParseConfigIntoObject(configName, "DATACONVERTER", dataLoaderOptions)
                dataLoaderOptions.VerifySettings();
                dataConverter = DataConverterBuilder.GetDataConverter(dataLoaderOptions.UseConverter)(dataLoaderOptions)
            
                print("Formatting Dataset")
                dataConverter.ConvertData()

                protonetOptions = NetOptions()
                self._ParseConfigIntoObject(self.BaseConfig, "NETTRAINER", protonetOptions)
                self._ParseConfigIntoObject(configName, "NETTRAINER", protonetOptions)
                protonetOptions.VerifySettings();
                protonetOptions.experiment_root = os.path.join(self.ExperimentResultsDir, timestamp, expName)
                protonet = NetTrainerBuilder.GetNetTrainer(protonetOptions.trainer_name)(protonetOptions, DatasetBuilder.GetDataset(protonetOptions.dataset_name))

                print("Training Model")
                bestTrainAcc = protonet.Train();
                print("Best train acc: " + str(bestTrainAcc))

                print("Testing Model")
                bestTestAcc = protonet.Test();
                results[expName] = bestTestAcc;
                print("Avg test acc: " + str(bestTestAcc))

                print("Copying dataset...")
                shutil.make_archive(os.path.join(self.ExperimentResultsDir, timestamp, expName + "-dataset"), 'zip', dataLoaderOptions.FormatedFolder)

                if self.GenerateGraphs is True:
                    print("Generating graphs...")
                    visualizer = ShapeletHistogramVisualiser(dataLoaderOptions.FormatedFolder)
                    allVisual = visualizer.VisualizeAllClasses();
                    allVisual.savefig(os.path.join(self.ExperimentResultsDir, timestamp, expName, "allVisual.png"))

                    if dataLoaderOptions.UseConverter == "ShapeletHistogramConverter":
                        shapelets = visualizer.VisualiseShapelets();
                        shapelets.savefig(os.path.join(self.ExperimentResultsDir, timestamp, expName, "allShapelets.png"))

                    if self.GenerateClassGraphs is True and dataLoaderOptions.UseConverter == "ShapeletHistogramConverter":
                        for classId in os.listdir(os.path.join(dataLoaderOptions.FormatedFolder, "data")):
                            print("Generating class " + classId + " graph...")
                            classfig = visualizer.VisualizeClass(int(classId));
                            classfig.savefig(os.path.join(self.ExperimentResultsDir, timestamp, expName, "class" + classId + ".png"))

                csvWriter.writerow([expName, dataLoaderOptions.UseConverter, protonetOptions.trainer_name, bestTrainAcc, bestTestAcc])

                print("   === " + expName + " ended ===   ")
                counter += 1;

        if self.GenerateGraphs is True:
            print("Generating full experiment graphs...")
            combiner = CSVResultsCombiner();
            fullResults = combiner.Combine(
                ["../ComparisonData/6shot.csv"],
                [{self.ExperimentName: results}],
                True
                );
            fullVisualiser = ResultsVisualiser();
            full = fullVisualiser.VisualiseAll(fullResults);
            full.savefig(os.path.join(self.ExperimentResultsDir, timestamp, "accuracies.png"))

        print("Experiments finished!")
        return {self.ExperimentName: results};

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