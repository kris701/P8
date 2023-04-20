import os
import csv
import time
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
from Helpers import TimeHelpers
from Helpers import ReflexionHelper

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
                options : ExperimentOptions = ExperimentOptions();
                ReflexionHelper.ParseConfigIntoObject(item, "SUITEOPTIONS", options)
                options.ExperimentResultsDir = os.path.join(options.ExperimentResultsDir, itemName + " - " + timestamp);
                os.makedirs(os.path.join(options.ExperimentResultsDir))
                self.RunExperiments(options);
            except Exception as e:
                self._LogPrint("An error occured in the execution of (" + item + ")", "error.txt")
                self._LogPrint("The error message was:", "error.txt")
                self._LogPrint("", "error.txt")
                self._LogPrint(str(e), "error.txt")
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

        with open(os.path.join(self.Options.ExperimentResultsDir, "comparable.csv"), 'w', newline='') as comparableCSV:
            comparableCsvWriter = csv.writer(comparableCSV, delimiter=',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
            comparableCsvWriter.writerow(['datasetName', 'NumberOfClasses', self.Options.ExperimentName])

            if self.Options.DebugMode is True:
                for expName in self.Options.ExperimentsToRun:
                    expName, avrTestAcc, nShot, nWay = self._RunExperiment(expName)
                    results[expName] = avrTestAcc;
                    comparableCsvWriter.writerow([expName, nWay, avrTestAcc]);
            else:
                data = []
                for expName in self.Options.ExperimentsToRun:
                    data.append(expName);

                poolResults = multiprocessing.Pool(self.Options.MaxProcessesToSpawn).map(self._RunExperiment, data)
                for expName, avrTestAcc, nShot, nWay in poolResults:
                    results[expName] = avrTestAcc;
                    comparableCsvWriter.writerow([expName, nWay, avrTestAcc]);

        if self.Options.GenerateGraphs is True and self.Options.GenerateAccuracyGraphs is True:
            self._LogPrint("Generating full experiment graphs...")
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
        
        self._LogPrint("   === " + expName + " ended (took " + TimeHelpers.ConvertSecToTimeFormat(time_lapsed) + ") ===   ")

        avrTestAcc = avrTestAcc / self.Options.ExperimentRounds;

        return expName, avrTestAcc, nShots, nWay;

    def _SetupDataConverter(self, configName : str) -> tuple[DataConverterOptions,BaseDataConverter]:
        dataLoaderOptions = DataConverterOptions()
        ReflexionHelper.ParseConfigIntoObject(self.Options.BaseConfig, "DATACONVERTER", dataLoaderOptions)
        ReflexionHelper.ParseConfigIntoObject(configName, "DATACONVERTER", dataLoaderOptions)
        dataLoaderOptions.VerifySettings();
        dataConverter = DataConverterBuilder.GetDataConverter(dataLoaderOptions.UseConverter)(dataLoaderOptions, self.Options.DebugMode)

        return dataLoaderOptions, dataConverter

    def _SetupNetTrainer(self, configName : str, roundResultDir : str) -> tuple[NetOptions,BaseNetTrainer]:
        protonetOptions = NetOptions()
        ReflexionHelper.ParseConfigIntoObject(self.Options.BaseConfig, "NETTRAINER", protonetOptions)
        ReflexionHelper.ParseConfigIntoObject(configName, "NETTRAINER", protonetOptions)
        protonetOptions.VerifySettings();
        protonetOptions.experiment_root = roundResultDir
        protonet = NetTrainerBuilder.GetNetTrainer(protonetOptions.trainer_name)(protonetOptions, DatasetBuilder.GetDataset(protonetOptions.dataset_name), self.Options.DebugMode)

        return protonetOptions, protonet

    def _RunRound(self, expName : str, step : int, configName : str) -> tuple[float,int,int]:
        self._LogPrint("[" + expName + "] Round " + str(step + 1) + " of " + str(self.Options.ExperimentRounds))
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

        if self.Options.GenerateClassGraphs:
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

        if self.Options.GenerateClassAccuracyGraph:
            self._DPrint("Generating class accuracy graph...")
            classAccGraph = visualizer.VisualizeDictionary(classAcc, "Accuracy Pr Class");
            classAccGraph.savefig(os.path.join(roundResultDir, "classAccuracy.png"))
            plt.close(classAccGraph)

        if self.Options.GenerateSourceGraphs:
            self._DPrint("Generating source graphs...")
            sourceVisual = visualizer.VisualiseSourceData();
            sourceVisual.savefig(os.path.join(roundResultDir, "source.png"))
            plt.close(sourceVisual)

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