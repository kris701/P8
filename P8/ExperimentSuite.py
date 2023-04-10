import os
import csv
import time
import configparser
import shutil
import multiprocessing

from DataConverters.DataConverterOptions import DataConverterOptions
from DataConverters import DataConverterBuilder
from NetTrainers.NetOptions import NetOptions
from NetTrainers import NetTrainerBuilder
from Datasets import DatasetBuilder
from DataVisualisers.ShapeletHistogramVisualiser import ShapeletHistogramVisualiser
from DataVisualisers.ResultsVisualiser import ResultsVisualiser
from ResultsCombiners.CSVResultsCombiner import CSVResultsCombiner
from ExperimentOptions import ExperimentOptions

class ExperimentSuite():
    Options : ExperimentOptions;

    def __init__(self, options : ExperimentOptions) -> None:
        self.Options = options

    def RunExperiments(self) -> dict:
        print("Running experiments...")
        results = {};
        timestamp = time.strftime("%Y%m%d-%H%M%S")
        os.makedirs(os.path.join(self.Options.ExperimentResultsDir, timestamp))

        print("Running " + str(len(self.Options.ExperimentsToRun)) + " experiments.")
        if self.Options.DebugMode is False:
            print("No debug info will be printed.")
        else:
            print("Full debug info will be printed.")
        print("")

        with open(os.path.join(self.Options.ExperimentResultsDir, timestamp, "comparable.csv"), 'w', newline='') as comparableCSV:
            comparableCsvWriter = csv.writer(comparableCSV, delimiter=',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
            comparableCsvWriter.writerow(['datasetName', 'NumberOfClasses', self.Options.ExperimentName])

            if self.Options.DebugMode is False:
                data = []
                for expName in self.Options.ExperimentsToRun:
                    data.append((expName, timestamp));

                poolResults = multiprocessing.Pool(len(self.Options.ExperimentsToRun)).map(self._RunExperiment, data)
                for expName, avrTestAcc, nShot, nWay in poolResults:
                    results[expName] = avrTestAcc;
                    comparableCsvWriter.writerow([expName, nWay, avrTestAcc]);
            else:
                for expName in self.Options.ExperimentsToRun:
                    expName, avrTestAcc, nShot, nWay = self._RunExperiment((expName, timestamp))
                    results[expName] = avrTestAcc;
                    comparableCsvWriter.writerow([expName, nWay, avrTestAcc]);

        if self.Options.GenerateGraphs is True:
            print("Generating full experiment graphs...")
            combiner = CSVResultsCombiner();
            fullResults = combiner.Combine(
                self.Options.ComparisonData,
                [{self.Options.ExperimentName: results}],
                True
                );
            fullVisualiser = ResultsVisualiser();
            full = fullVisualiser.VisualiseAll(fullResults);
            full.savefig(os.path.join(self.Options.ExperimentResultsDir, timestamp, "accuracies.png"))

        print("Experiments finished!")
        return {self.Options.ExperimentName: results};

    def _RunExperiment(self, data):
        expName, timestamp = data
        print("   === " + expName + " started ===   ")
        start_time = time.time()

        configName = os.path.join(self.Options.ExperimentConfigDir, expName + ".ini")

        avrTestAcc : float = 0;

        for step in range(0, self.Options.ExperimentRounds):
            print("[" + expName + "] Round " + str(step) + " of " + str(self.Options.ExperimentRounds))
            roundResultDir : str = os.path.join(self.Options.ExperimentResultsDir, step);

            dataLoaderOptions = DataConverterOptions()
            self._ParseConfigIntoObject(self.Options.BaseConfig, "DATACONVERTER", dataLoaderOptions)
            self._ParseConfigIntoObject(configName, "DATACONVERTER", dataLoaderOptions)
            dataLoaderOptions.VerifySettings();
            dataConverter = DataConverterBuilder.GetDataConverter(dataLoaderOptions.UseConverter)(dataLoaderOptions, self.Options.DebugMode)
          
            if self.Options.ForceRemakeDataset:
                if self.Options.DebugMode is True: print("Force removing old dataset")
                if os.path.exists(dataLoaderOptions.FormatedFolder):
                    shutil.rmtree(dataLoaderOptions.FormatedFolder);

            if self.Options.DebugMode is True: print("Formatting Dataset")
            dataConverter.ConvertData()

            protonetOptions = NetOptions()
            self._ParseConfigIntoObject(self.Options.BaseConfig, "NETTRAINER", protonetOptions)
            self._ParseConfigIntoObject(configName, "NETTRAINER", protonetOptions)
            protonetOptions.VerifySettings();
            protonetOptions.experiment_root = os.path.join(roundResultDir, timestamp, expName)
            protonet = NetTrainerBuilder.GetNetTrainer(protonetOptions.trainer_name)(protonetOptions, DatasetBuilder.GetDataset(protonetOptions.dataset_name), self.Options.DebugMode)

            nShots = dataLoaderOptions.TestClassesSplit;
            nWay = protonetOptions.classes_per_it_tr;

            if self.Options.DebugMode is True: print("Training Model")
            bestTrainAcc = protonet.Train();
            if self.Options.DebugMode is True: print("Best train acc: " + str(bestTrainAcc))

            if self.Options.DebugMode is True: print("Testing Model")
            bestTestAcc = protonet.Test();
            avrTestAcc += bestTestAcc;
            if self.Options.DebugMode is True: print("Avg test acc: " + str(bestTestAcc))

            if self.Options.ZipDataset:
                if self.Options.DebugMode is True: print("Copying dataset...")
                shutil.make_archive(os.path.join(roundResultDir, timestamp, expName + "-dataset"), 'zip', dataLoaderOptions.FormatedFolder)

            if self.Options.CopyConfigs:
                if self.Options.DebugMode is True: print("Copying configs...")
                shutil.copyfile(self.Options.BaseConfig, os.path.join(roundResultDir, timestamp, expName, "baseConfig.ini"));
                shutil.copyfile(configName, os.path.join(roundResultDir, timestamp, expName, "config.ini"));

            if self.Options.GenerateGraphs is True:
                visualizer = ShapeletHistogramVisualiser(dataLoaderOptions.FormatedFolder)
                if self.Options.GenerateExperimentGraph is True:
                    if self.Options.DebugMode is True: print("Generating experiment graphs...")
                    allVisual = visualizer.VisualizeAllClasses();
                    allVisual.savefig(os.path.join(roundResultDir, timestamp, expName, "allVisual.png"))

                if self.Options.GenerateShapeletGraphs:
                    if self.Options.DebugMode is True: print("Generating shapelet graphs...")
                    if dataLoaderOptions.UseConverter == "ShapeletHistogramConverter":
                        shapelets = visualizer.VisualiseShapelets();
                        shapelets.savefig(os.path.join(roundResultDir, timestamp, expName, "allShapelets.png"))

                if self.Options.GenerateClassGraphs is True and dataLoaderOptions.UseConverter == "ShapeletHistogramConverter":
                    if self.Options.DebugMode is True: print("Generating class graphs...")
                    for classId in os.listdir(os.path.join(dataLoaderOptions.FormatedFolder, "data")):
                        if self.Options.DebugMode is True: print("Generating class " + classId + " graph...")
                        classfig = visualizer.VisualizeClass(int(classId));
                        classfig.savefig(os.path.join(roundResultDir, timestamp, expName, "class" + classId + ".png"))

                if self.Options.GenerateSourceGraphs:
                    if self.Options.DebugMode is True: print("Generating source graphs...")
                    sourceVisual = visualizer.VisualiseSourceData();
                    sourceVisual.savefig(os.path.join(roundResultDir, timestamp, expName, "source.png"))

        end_time = time.time()
        time_lapsed = end_time - start_time
        
        print("   === " + expName + " ended (took " + self._TimeConvert(time_lapsed) + ") ===   ")

        avrTestAcc = avrTestAcc / self.Options.ExperimentRounds;

        return expName, avrTestAcc, nShots, nWay;

    def _TimeConvert(self,sec) -> str:
        mins = sec // 60
        sec = sec % 60
        hours = mins // 60
        mins = mins % 60
        return "{0}:{1}:{2}".format(int(hours),int(mins),sec)

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