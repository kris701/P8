import os
import time
import shutil
import multiprocessing
import gc
import matplotlib
import traceback

matplotlib.use('Agg')

from DataConverters.DataConverterOptions import DataConverterOptions
from DataConverters.DirectDataConverter import BaseDataConverter
from DataConverters.ShapeletHistogramConverter import ShapeletHistogramConverter
from DataConverters import DataConverterBuilder
from NetTrainers.NetOptions import NetOptions
from NetTrainers import NetTrainerBuilder
from NetTrainers.BaseNetTrainer import BaseNetTrainer
from NetTrainers.NaiveKNN.NetTrainer import NetTrainer
from Datasets import DatasetBuilder
from DataVisualisers.ShapeletHistogramVisualiser import ShapeletHistogramVisualiser
from DataVisualisers.ResultsVisualiser import ResultsVisualiser
from ExperimentOptions import ExperimentOptions
from Helpers import TimeHelpers
from Helpers import ReflexionHelper
from Helpers import ComparisonDataHelper
from Helpers import CSVHelper

class ExperimentSuite():
    Options : ExperimentOptions;
    SuiteConfigPath : str = "Experiments/SuiteConfigs/";

    def __init__(self, suiteConfigPath : str) -> None:
        self.SuiteConfigPath = suiteConfigPath;

    def CheckIfQueueIsValid(self, queue : list):
        for item in queue:
            configItem = os.path.join(self.SuiteConfigPath, item)

            # Check if queue item exist
            if not os.path.exists(configItem):
                raise FileNotFoundError("The config queue item '" + configItem + "' was not found!")

            # Check if the item can be parsed
            options : ExperimentOptions = ExperimentOptions();
            try:
                ReflexionHelper.ParseConfigIntoObject(configItem, "SUITEOPTIONS", options)
            except Exception as e:
                raise Exception("Cannot parse the queue config file: " + configItem);

            # Check if base config is there
            if not os.path.exists(options.BaseConfig):
                raise FileNotFoundError("The base experiment config item '" + options.BaseConfig + "' was not found!")

            # Check if the base config can be parsed
            try:
                dataLoaderOptions = DataConverterOptions()
                ReflexionHelper.ParseConfigIntoObject(options.BaseConfig, "DATACONVERTER", dataLoaderOptions)

                protonetOptions = NetOptions()
                ReflexionHelper.ParseConfigIntoObject(options.BaseConfig, "NETTRAINER", protonetOptions)
            except Exception as e:
                raise Exception("Cannot parse the base config file: " + options.BaseConfig);

            for experiment in options.ExperimentsToRun:
                configName = os.path.join(options.ExperimentConfigDir, experiment + ".ini")

                # Check if Experiment exists
                if not os.path.exists(configName):
                    raise FileNotFoundError("The experiment config item '" + configName + "' was not found!")

                # Check if the experiment config can be parsed
                try:
                    dataLoaderOptions = DataConverterOptions()
                    ReflexionHelper.ParseConfigIntoObject(options.BaseConfig, "DATACONVERTER", dataLoaderOptions)
                    ReflexionHelper.ParseConfigIntoObject(configName, "DATACONVERTER", dataLoaderOptions)

                    protonetOptions = NetOptions()
                    ReflexionHelper.ParseConfigIntoObject(options.BaseConfig, "NETTRAINER", protonetOptions)
                    ReflexionHelper.ParseConfigIntoObject(configName, "NETTRAINER", protonetOptions)
                except Exception as e:
                    raise Exception("Cannot parse the experiment config file: " + options.BaseConfig);

                # Check if data files are there
                if not os.path.isfile(dataLoaderOptions.SourceTrainData):
                    raise FileNotFoundError("The experiment config item '" + configName + "' requires the dataset '" + dataLoaderOptions.SourceTrainData + "' to be available!")
                if not os.path.isfile(dataLoaderOptions.SourceTestData):
                    raise FileNotFoundError("The experiment config item '" + configName + "' requires the dataset '" + dataLoaderOptions.SourceTestData + "' to be available!")

    def _CheckForRecompilation(self) -> None:
        converter = ShapeletHistogramConverter(DataConverterOptions(), False);
        if converter._ShouldRecompile():
            converter._CompileFeatureExtractor();

        knn = NetTrainer(NetOptions(), None, False)
        if knn._ShouldRecompile():
            knn._CompileFeatureExtractor();

    def RunExperimentQueue(self, queue : list, throwOnError : bool = False):
        print("Checking if queue is valid...")
        self.CheckIfQueueIsValid(queue)
        print("Queue is valid!")

        print("Cheking if anything needs to compile...")
        self._CheckForRecompilation();
        print("Compile check done!")

        print("Experiment Suite Queue started...")
        print("There is a total of " + str(len(queue)) + " items in the queue")
        counter : int = 1;
        for item in queue:
            print("Queue item " + str(counter) + " out of " + str(len(queue)) + " started!")
            print("Queue config: " + item)
            configItem = os.path.join(self.SuiteConfigPath, item)
            try:
                timestamp = time.strftime("%Y%m%d-%H%M%S")
                itemName = item.replace(".ini","")
                options : ExperimentOptions = ExperimentOptions();
                ReflexionHelper.ParseConfigIntoObject(configItem, "SUITEOPTIONS", options)
                options.ExperimentResultsDir = os.path.join(options.ExperimentResultsDir, itemName + " - " + timestamp);
                os.makedirs(os.path.join(options.ExperimentResultsDir))
                self.RunExperiments(options);
            except Exception as e:
                if throwOnError:
                    raise e;
                self._LogPrint("An error occured in the execution of (" + configItem + ")", "error.txt")
                self._LogPrint("The error message was:", "error.txt")
                self._LogPrint("", "error.txt")
                self._LogPrint(str(e), "error.txt")
                self._LogPrint("", "error.txt")
                self._LogPrint("Traceback:", "error.txt")
                self._LogPrint(traceback.format_exc())
            print("Queue item " + str(counter) + " ended!")
            counter += 1;
        print("Experiment Suite Queue finised!")

    def RunExperiments(self, options : ExperimentOptions) -> dict:
        self.Options = options
        self._LogPrint("Running experiments...")
        results = {};

        self._LogPrint("Running " + str(len(self.Options.ExperimentsToRun)) + " experiments.")
        if self.Options.DebugMode:
            self._LogPrint("Full debug info will be printed.")
        else:
            self._LogPrint("No debug info will be printed.")
        self._LogPrint("")

        self._DPrint("Checking if config files exist")
        for configName in self.Options.ExperimentsToRun:
            configName = os.path.join(self.Options.ExperimentConfigDir, configName + ".ini")
            if not os.path.exists(configName):
                raise FileNotFoundError("Config '" + configName + "' not found!")

        csvPath = os.path.join(self.Options.ExperimentResultsDir, "comparable.csv");
        CSVHelper.AppendToCSV(['datasetName', 'NumberOfClasses', self.Options.ExperimentName], csvPath);

        if self.Options.DebugMode is True:
            for expName in self.Options.ExperimentsToRun:
                expName, avrTestAcc, nShot, nWay = self._RunExperiment(expName)
                results[expName] = avrTestAcc;
                CSVHelper.AppendToCSV([expName, nWay, avrTestAcc], csvPath);
        else:
            data = []
            for expName in self.Options.ExperimentsToRun:
                data.append(expName);

            poolResults = multiprocessing.Pool(self.Options.MaxProcessesToSpawn).map(self._RunExperiment, data)
            for expName, avrTestAcc, nShot, nWay in poolResults:
                results[expName] = avrTestAcc;
                CSVHelper.AppendToCSV([expName, nWay, avrTestAcc], csvPath);

        if self.Options.GenerateGraphs is True and self.Options.GenerateAccuracyGraphs is True:
            self._LogPrint("Generating full experiment accuracy graphs...")
            fullResults = ComparisonDataHelper.CombineDictionaries(
                self.Options.ComparisonData,
                [{self.Options.ExperimentName: results}],
                True
                );
            fullVisualiser = ResultsVisualiser("None");
            fullVisualiser.SaveAndClose(
                fullVisualiser.VisualiseAccuracy(fullResults, "Accuracy Pr Dataset (" + options.ExperimentName + ")"),
                os.path.join(self.Options.ExperimentResultsDir, "accuracies.png"))
            fullVisualiser.SaveAndClose(
                fullVisualiser.VisualiseAverageAccuracy(fullResults, "Accuracy Pr Method (" + options.ExperimentName + ")"),
                os.path.join(self.Options.ExperimentResultsDir, "average_accuracies.png"))

        self._LogPrint("Experiments finished!")
        return {self.Options.ExperimentName: results};

    def _RunExperiment(self, expName : str):
        self._LogPrint("   === " + expName + " started ===   ")
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
        
        self._LogPrint("   === " + expName + " ended (took " + TimeHelpers.ConvertSecToTimeFormat(int(time_lapsed)) + ") ===   ")

        avrTestAcc = avrTestAcc / self.Options.ExperimentRounds;

        return expName, avrTestAcc, nShots, nWay;

    def _SetupDataConverter(self, configName : str, baseConfig : str, isDebugMode : bool) -> tuple[DataConverterOptions,BaseDataConverter]:
        dataLoaderOptions = DataConverterOptions()
        ReflexionHelper.ParseConfigIntoObject(baseConfig, "DATACONVERTER", dataLoaderOptions)
        ReflexionHelper.ParseConfigIntoObject(configName, "DATACONVERTER", dataLoaderOptions)
        dataLoaderOptions.VerifySettings();
        dataConverter = DataConverterBuilder.GetDataConverter(dataLoaderOptions.UseConverter)(dataLoaderOptions, isDebugMode)

        return dataLoaderOptions, dataConverter

    def _SetupNetTrainer(self, configName : str, roundResultDir : str, baseConfig : str, isDebugMode : bool) -> tuple[NetOptions,BaseNetTrainer]:
        protonetOptions = NetOptions()
        ReflexionHelper.ParseConfigIntoObject(baseConfig, "NETTRAINER", protonetOptions)
        ReflexionHelper.ParseConfigIntoObject(configName, "NETTRAINER", protonetOptions)
        protonetOptions.VerifySettings();
        protonetOptions.experiment_root = roundResultDir
        protonet = NetTrainerBuilder.GetNetTrainer(protonetOptions.trainer_name)(protonetOptions, DatasetBuilder.GetDataset(protonetOptions.dataset_name), isDebugMode)

        return protonetOptions, protonet

    def _RunRound(self, expName : str, step : int, configName : str) -> tuple[float,int,int]:
        self._LogPrint("      [" + expName + "] Round " + str(step + 1) + " of " + str(self.Options.ExperimentRounds))
        roundResultDir : str = os.path.join(self.Options.ExperimentResultsDir, expName, str(step + 1));

        dataLoaderOptions, dataConverter = self._SetupDataConverter(configName, self.Options.BaseConfig, self.Options.DebugMode);
          
        if self.Options.ForceRemakeDataset:
            self._DPrint("Force removing old dataset")
            if os.path.exists(dataLoaderOptions.FormatedFolder):
                shutil.rmtree(dataLoaderOptions.FormatedFolder);

        if self.Options.FormatDataset:
            self._DPrint("Formatting Dataset")
            dataConverter.ConvertData()

        protonetOptions, protonet = self._SetupNetTrainer(configName, roundResultDir, self.Options.BaseConfig, self.Options.DebugMode);

        nShots = dataLoaderOptions.TestClassesSplit;
        nWay = protonetOptions.classes_per_it_tr;

        bestTrainAcc = 0;
        if self.Options.RunTrain:
            self._DPrint("Training Model")
            bestTrainAcc = protonet.Train();
            self._DPrint("Best train acc: " + str(bestTrainAcc))

        bestTestAcc = 0;
        classAcc = {}
        if self.Options.RunTest:
            self._DPrint("Testing Model")
            bestTestAcc, classAcc = protonet.Test();
            self._DPrint("Avg test acc: " + str(bestTestAcc))

        if self.Options.ZipDataset:
            self._DPrint("Copying dataset...")
            shutil.make_archive(os.path.join(roundResultDir, expName + "-dataset"), 'zip', dataLoaderOptions.FormatedFolder)

        if self.Options.CopyConfigs:
            self._DPrint("Copying configs...")
            shutil.copyfile(self.Options.BaseConfig, os.path.join(roundResultDir, "baseConfig.ini"));
            shutil.copyfile(configName, os.path.join(roundResultDir, "config.ini"));

        if self.Options.GenerateGraphs:
            self._GenerateRoundGraphs(dataLoaderOptions, roundResultDir, classAcc);

        gc.collect();

        return (bestTestAcc, nShots, nWay)

    def _GenerateRoundGraphs(self, dataLoaderOptions : BaseDataConverter, roundResultDir : str, classAcc : dict) -> None:
        visualizer = ShapeletHistogramVisualiser(dataLoaderOptions.FormatedFolder)
        if self.Options.GenerateExperimentGraph:
            self._DPrint("Generating graphs of all classes combined... (TRAIN)")
            visualizer.SaveAndClose(
                visualizer.VisualizeAllClasses(True,False),
                os.path.join(roundResultDir, "train_allVisual.png"))
            self._DPrint("Generating graphs of all classes combined... (TEST)")
            visualizer.SaveAndClose(
                visualizer.VisualizeAllClasses(False,True),
                os.path.join(roundResultDir, "test_allVisual.png"))
            self._DPrint("Generating graphs of all classes combined... (BOTH)")
            visualizer.SaveAndClose(
                visualizer.VisualizeAllClasses(True,True),
                os.path.join(roundResultDir, "allVisual.png"))

        if self.Options.GenerateShapeletGraphs:
            self._DPrint("Generating shapelet graphs...")
            visualizer.SaveAndClose(
                visualizer.VisualiseIndividualDatapoints(os.path.join(dataLoaderOptions.FormatedFolder, "features", "shapelets"), "Shapelets"),
                os.path.join(roundResultDir, "allShapelets.png"))

        if self.Options.GenerateClassGraphs:
            self._DPrint("Generating class graphs...")
            for classId in os.listdir(os.path.join(dataLoaderOptions.FormatedFolder, "data")):
                self._DPrint("  Generating class " + classId + " graph... (TRAIN)")
                visualizer.SaveAndClose(
                    visualizer.VisualizeClass(int(classId), True, False),
                    os.path.join(roundResultDir, "train_class" + classId + ".png"))
                self._DPrint("  Generating class " + classId + " graph... (TEST)")
                visualizer.SaveAndClose(
                    visualizer.VisualizeClass(int(classId), False, True),
                    os.path.join(roundResultDir, "test_class" + classId + ".png"))
                self._DPrint("  Generating class " + classId + " graph... (BOTH)")
                visualizer.SaveAndClose(
                    visualizer.VisualizeClass(int(classId)),
                    os.path.join(roundResultDir, "class" + classId + ".png"))

        if self.Options.GenerateClassAccuracyGraph:
            self._DPrint("Generating class accuracy graph...")
            visualizer.SaveAndClose(
                visualizer.VisualizeDictionary(classAcc, "Accuracy Pr Class", "Class: ", "Class ID", "Accuracy"),
                os.path.join(roundResultDir, "classAccuracy.png"))

        if self.Options.GenerateOriginalSourceGraph:
            self._DPrint("Generating original source graphs...")
            visualizer.SaveAndClose(
                visualizer.VisualizeCombinedDatapoints(os.path.join(dataLoaderOptions.FormatedFolder, "source", "original"), "Source Data (Original)"),
                os.path.join(roundResultDir, "originalSource.png"))

        if self.Options.GenerateAugmentedSourceGraph:
            self._DPrint("Generating augmented source graphs...")
            visualizer.SaveAndClose(
                visualizer.VisualizeCombinedDatapoints(os.path.join(dataLoaderOptions.FormatedFolder, "source", "augmentation"), "Source Data (Augmented)"),
                os.path.join(roundResultDir, "augmentedSource.png"))

        if self.Options.GenerateCandidatesGraph:
            self._DPrint("Generating candidate purge graphs...")
            visualizer.SaveAndClose(
                visualizer.VisualizeCombinedDatapoints(os.path.join(dataLoaderOptions.FormatedFolder, "purged", "candidates"), "Purge Candidate Data"),
                os.path.join(roundResultDir, "purgeCandidates.png"))

        if self.Options.GenerateRejectsGraph:
            self._DPrint("Generating reject purge graphs...")
            visualizer.SaveAndClose(
                visualizer.VisualizeCombinedDatapoints(os.path.join(dataLoaderOptions.FormatedFolder, "purged", "rejects"), "Purge Reject Data"),
                os.path.join(roundResultDir, "purgeRejects.png"))

    def _DPrint(self, text : str, logName : str = "log.txt") -> None:
        if self.Options.DebugMode:
            print(text);
        logFile = os.path.join(self.Options.ExperimentResultsDir, logName)
        if not os.path.exists(logFile):
            open(logFile, "x")
        with open(logFile, 'a') as file:
            file.write(text + "\n")

    def _LogPrint(self, text : str, logName : str = "log.txt") -> None:
        print(text);
        logFile = os.path.join(self.Options.ExperimentResultsDir, logName)
        if not os.path.exists(logFile):
            open(logFile, "x")
        with open(logFile, 'a') as file:
            file.write(text + "\n")