import os

from DataConverters.DataConverter import DataWriter as dw
from DataConverters.DataConverterOptions import DataConverterOptions as dwo
from NetTrainers.ProtoNetTrainer import NetTrainer as pnt
from NetTrainers.ProtoNetTrainer import NetOptions as pntOpt
from Datasets.SwedishLeaf import SwedishLeafDataset

targetDataDir = "formated" + os.sep + "swedishLeaf"
targetOutputDir = "output" + os.sep + "swedishLeaf"

# Convert data into a new format
dataOptions = dwo()
dataOptions.FormatedFolder = targetDataDir
dataOptions.TrainValSplit = 0.7;
dataOptions.TestClassesSplit = 0.2;
dataOptions.SourceTrainData = "./Data/SwedishLeaf/SwedishLeaf_TRAIN.tsv";
dataOptions.SourceTestData = "./Data/SwedishLeaf/SwedishLeaf_TEST.tsv";

dataConverter = dw(dataOptions);
dataConverter.ConvertData()

# Setup protonet 
options = pntOpt.NetOptions
options.dataset_root = targetDataDir
options.experiment_root = targetOutputDir;
options.classes_per_it_tr = 10
options.classes_per_it_test = 3
options.train_epochs = 5;
options.test_epochs = 1;

net = pnt.NetTrainer(options, SwedishLeafDataset)
print("Training Model (train set size: {}, validate set size: {})".format(
    len(net.TrainDataloader.dataset.all_items), 
    len(net.ValDataloader.dataset.all_items)))
best_train_acc = net.Train();
print("Best train acc: {:0.02f}".format(best_train_acc))
print("")
print("Testing Model (test set size: {})".format(
    len(net.TestDataloader.dataset.all_items)))
avr_test_acc = net.Test();
print("Avr test acc: {:0.02f}".format(avr_test_acc))
