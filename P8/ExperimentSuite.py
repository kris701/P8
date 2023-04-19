import os
import csv
import time
import configparser
import shutil
import multiprocessing
import gc
import matplotlib
import matplotlib.pyplot as plt

matplotlib.use('Agg')

from DataConverters.DataConverterOptions import DataConverterOptions
from DataConverters.DirectDataConverter import BaseDataConverter
from DataConverters import DataConverterBuilder
from NetTrainers.NetOptions import NetOptions
from NetTrainers import NetTrainerBuilder
from NetTrainers.BaseNetTrainer import BaseNetTrainer
from Datasets import DatasetBuilder
from DataVisualisers.ShapeletHistogramVisualiser import ShapeletHistogramVisualiser
from DataVisualisers.ResultsVisualiser import ResultsVisualiser
from ResultsCombiners.CSVResultsCombiner import CSVResultsCombiner
from ExperimentOptions import ExperimentOptions

class ExperimentSuite():
    Options : ExperimentOptions;

    def RunExperimentQueue(self, queue : list):
        print("Experiment Suite Queue started...")
        print("There is a total of " + str(len(queue)) + " items in the queue")
        counter : int = 1;
        for item in queue:
            print("Queue item " + str(counter) + " out of " + str(len(queue)) + " started!")
            try:
                timestamp = time.strftime("%Y%m%d-%H%M%S")
                itemName = item.rsplit("/", 1)[1].replace(".ini","")
                option : ExperimentOptions = ExperimentOptions();
                self._ParseConfigIntoObject(item, "SUITEOPTIONS", option)
                option.ExperimentResultsDir = os.path.join(option.ExperimentResultsDir, itemName + " - " + timestamp);
                self.RunExperiments(option);
            except:
                print("An error occured in the execution of (" + item + ")")
            print("Queue item " + str(counter) + " ended!")
            counter += 1;
        print("Experiment Suite Queue finised!")

    def RunExperiments(self, options : ExperimentOptions) -> dict:
        self.Options = options
        print("Running experiments...")
        results = {};
        os.makedirs(os.path.join(self.Options.ExperimentResultsDir))

        print("Running " + str(len(self.Options.ExperimentsToRun)) + " experiments.")
        if self.Options.DebugMode:
            print("Full debug info will be printed.")
        else:
            print("No debug info will be printed.")
        print("")

        with open(os.path.join(self.Options.ExperimentResultsDir, "comparable.csv"), 'w', newline='') as comparableCSV:
            comparableCsvWriter = csv.writer(comparableCSV, delimiter=',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
            comparableCsvWriter.writerow(['datasetName', 'NumberOfClasses', self.Options.ExperimentName])

            if self.Options.DebugMode is False:
                poolResults = multiprocessing.Pool(len(self.Options.ExperimentsToRun)).map(self._RunExperiment, expName)
                for expName, avrTestAcc, nShot, nWay in poolResults:
                    results[expName] = avrTestAcc;
                    comparableCsvWriter.writerow([expName, nWay, avrTestAcc]);
            else:
                for expName in self.Options.ExperimentsToRun:
                    expName, avrTestAcc, nShot, nWay = self._RunExperiment(expName)
                    results[expName] = avrTestAcc;
                    comparableCsvWriter.writerow([expName, nWay, avrTestAcc]);

        if self.Options.GenerateGraphs is True and self.Options.GenerateAccuracyGraphs is True:
            print("Generating full experiment graphs...")
            combiner = CSVResultsCombiner();
            fullResults = combiner.Combine(
                self.Options.ComparisonData,
                [{self.Options.ExperimentName: results}],
                True
                );
            fullVisualiser = ResultsVisualiser();
            full = fullVisualiser.VisualiseAll(fullResults);
            full.savefig(os.path.join(self.Options.ExperimentResultsDir, "accuracies.png"))
            plt.close(full)

        print("Experiments finished!")
        return {self.Options.ExperimentName: results};

    def _RunExperiment(self, expName : str):
        print("   === " + expName + " started ===   ")
        start_time = time.time()

        configName = os.path.join(self.Options.ExperimentConfigDir, expName + ".ini")

        avrTestAcc : float = 0;
        nShots : int = 0;
        nWay : int = 0;

        for step in range(0, self.Options.ExperimentRounds):
            bestTestAcc, shots, ways = self._RunRound(expName, step, configName);
            avrTestAcc += bestTestAcc;
            nShots = shots;
            nWay = ways

        end_time = time.time()
        time_lapsed = end_time - start_time
        
        print("   === " + expName + " ended (took " + self._TimeConvert(time_lapsed) + ") ===   ")

        avrTestAcc = avrTestAcc / self.Options.ExperimentRounds;

        return expName, avrTestAcc, nShots, nWay;

    def _SetupDataConverter(self, configName : str) -> tuple[DataConverterOptions,BaseDataConverter]:
        dataLoaderOptions = DataConverterOptions()
        self._ParseConfigIntoObject(self.Options.BaseConfig, "DATACONVERTER", dataLoaderOptions)
        self._ParseConfigIntoObject(configName, "DATACONVERTER", dataLoaderOptions)
        dataLoaderOptions.VerifySettings();
        dataConverter = DataConverterBuilder.GetDataConverter(dataLoaderOptions.UseConverter)(dataLoaderOptions, self.Options.DebugMode)

        return dataLoaderOptions, dataConverter

    def _SetupNetTrainer(self, configName : str, roundResultDir : str) -> tuple[NetOptions,BaseNetTrainer]:
        protonetOptions = NetOptions()
        self._ParseConfigIntoObject(self.Options.BaseConfig, "NETTRAINER", protonetOptions)
        self._ParseConfigIntoObject(configName, "NETTRAINER", protonetOptions)
        protonetOptions.VerifySettings();
        protonetOptions.experiment_root = roundResultDir
        protonet = NetTrainerBuilder.GetNetTrainer(protonetOptions.trainer_name)(protonetOptions, DatasetBuilder.GetDataset(protonetOptions.dataset_name), self.Options.DebugMode)

        return protonetOptions, protonet

    def _RunRound(self, expName : str, step : int, configName : str) -> tuple[float,int,int]:
        print("[" + expName + "] Round " + str(step + 1) + " of " + str(self.Options.ExperimentRounds))
        roundResultDir : str = os.path.join(self.Options.ExperimentResultsDir, expName, str(step + 1));

        dataLoaderOptions, dataConverter = self._SetupDataConverter(configName);
          
        if self.Options.ForceRemakeDataset:
            self._DPrint("Force removing old dataset")
            if os.path.exists(dataLoaderOptions.FormatedFolder):
                shutil.rmtree(dataLoaderOptions.FormatedFolder);

        if self.Options.FormatDataset:
            self._DPrint("Formatting Dataset")
            dataConverter.ConvertData()

        protonetOptions, protonet = self._SetupNetTrainer(configName, roundResultDir);

        nShots = dataLoaderOptions.TestClassesSplit;
        nWay = protonetOptions.classes_per_it_tr;

        bestTrainAcc = 0;
        if self.Options.RunTrain:
            self._DPrint("Training Model")
            bestTrainAcc = protonet.Train();
            self._DPrint("Best train acc: " + str(bestTrainAcc))

        bestTestAcc = 0;
        if self.Options.RunTest:
            self._DPrint("Testing Model")
            bestTestAcc = protonet.Test();
            self._DPrint("Avg test acc: " + str(bestTestAcc))

        if self.Options.ZipDataset:
            self._DPrint("Copying dataset...")
            shutil.make_archive(os.path.join(roundResultDir, expName + "-dataset"), 'zip', dataLoaderOptions.FormatedFolder)

        if self.Options.CopyConfigs:
            self._DPrint("Copying configs...")
            shutil.copyfile(self.Options.BaseConfig, os.path.join(roundResultDir, "baseConfig.ini"));
            shutil.copyfile(configName, os.path.join(roundResultDir, "config.ini"));

        if self.Options.GenerateGraphs:
            self._GenerateRoundGraphs(dataLoaderOptions, roundResultDir);

        gc.collect();

        return (bestTestAcc, nShots, nWay)

    def _GenerateRoundGraphs(self, dataLoaderOptions : BaseDataConverter, roundResultDir : str) -> None:
        visualizer = ShapeletHistogramVisualiser(dataLoaderOptions.FormatedFolder)
        if self.Options.GenerateExperimentGraph:
            self._DPrint("Generating graphs of all classes combined... (TRAIN)")
            allVisual_train = visualizer.VisualizeAllClasses(True,False);
            allVisual_train.savefig(os.path.join(roundResultDir, "train_allVisual.png"))
            plt.close(allVisual_train)
            self._DPrint("Generating graphs of all classes combined... (TEST)")
            allVisual_test = visualizer.VisualizeAllClasses(False,True);
            allVisual_test.savefig(os.path.join(roundResultDir, "test_allVisual.png"))
            plt.close(allVisual_test)
            self._DPrint("Generating graphs of all classes combined... (BOTH)")
            allVisual = visualizer.VisualizeAllClasses(True,True);
            allVisual.savefig(os.path.join(roundResultDir, "allVisual.png"))
            plt.close(allVisual)

        if self.Options.GenerateShapeletGraphs:
            self._DPrint("Generating shapelet graphs...")
            if dataLoaderOptions.UseConverter == "ShapeletHistogramConverter":
                shapelets = visualizer.VisualiseShapelets();
                shapelets.savefig(os.path.join(roundResultDir, "allShapelets.png"))
                plt.close(shapelets)

        if self.Options.GenerateClassGraphs is True and dataLoaderOptions.UseConverter == "ShapeletHistogramConverter":
            self._DPrint("Generating class graphs...")
            for classId in os.listdir(os.path.join(dataLoaderOptions.FormatedFolder, "data")):
                self._DPrint("  Generating class " + classId + " graph... (TRAIN)")
                classfig_train = visualizer.VisualizeClass(int(classId), True, False);
                classfig_train.savefig(os.path.join(roundResultDir, "train_class" + classId + ".png"))
                plt.close(classfig_train)
                self._DPrint("  Generating class " + classId + " graph... (TEST)")
                classfig_test = visualizer.VisualizeClass(int(classId), False, True);
                classfig_test.savefig(os.path.join(roundResultDir, "test_class" + classId + ".png"))
                plt.close(classfig_test)
                self._DPrint("  Generating class " + classId + " graph... (BOTH)")
                classfig = visualizer.VisualizeClass(int(classId));
                classfig.savefig(os.path.join(roundResultDir, "class" + classId + ".png"))
                plt.close(classfig)

        if self.Options.GenerateSourceGraphs:
            self._DPrint("Generating source graphs...")
            sourceVisual = visualizer.VisualiseSourceData();
            sourceVisual.savefig(os.path.join(roundResultDir, "source.png"))
            plt.close(sourceVisual)

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
                elif typeName == "list":
                    items = config[sectionName][index].split(",")
                    obj.__dict__[index] = items;
                else:
                    raise Exception("Invalid config type!")
            else:
                raise Warning("Warning, key '" + index + "' not in this object!")

    def _DPrint(self, text : str) -> None:
        if self.Options.DebugMode:
            print(text);