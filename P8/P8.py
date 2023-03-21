import os

from DataConverters.DirectDataConverter import DirectDataConverter
from DataConverters.ShapeletHistogramConverter import ShapeletHistogramConverter
from DataConverters.DataConverterOptions import DataConverterOptions
from DataConverters.ShapeletHistogramConverter.ShapeletHistogramConverterOptions import ShapeletHistogramConverterOptions
from NetTrainers.ProtoNetTrainer import NetTrainer
from NetTrainers.ProtoNetTrainer import NetOptions
from Datasets.SwedishLeaf import SwedishLeafDataset

targetDataDir = "formated" + os.sep + "swedishLeaf"
targetOutputDir = "output" + os.sep + "swedishLeaf"

# Convert data into a new format
dataOptions = ShapeletHistogramConverterOptions()
dataOptions.FormatedFolder = targetDataDir
dataOptions.TrainValSplit = 1;
dataOptions.TestClassesSplit = 0.05;
dataOptions.SourceTrainData = "./Data/SwedishLeaf/SwedishLeaf_TRAIN.tsv";
dataOptions.SourceTestData = "./Data/SwedishLeaf/SwedishLeaf_TEST.tsv";
dataOptions.depth = 3;
dataOptions.maxWindowSize = 8;

#dataConverter = DirectDataConverter(dataOptions);
dataConverter = ShapeletHistogramConverter(dataOptions);
dataConverter.ConvertData()

# Setup protonet 
options = NetOptions.NetOptions
options.dataset_root = targetDataDir
options.experiment_root = targetOutputDir;
options.classes_per_it_tr = 10
options.classes_per_it_test = 3
options.train_epochs = 5;
options.test_epochs = 1;

net = NetTrainer.NetTrainer(options, SwedishLeafDataset)

# Train the protonet
print("Training Model (train set size: {}, validate set size: {})".format(
    len(net.TrainDataloader.dataset.all_items), 
    len(net.ValDataloader.dataset.all_items)))
best_train_acc = net.Train();
print("Best train acc: {:0.02f}".format(best_train_acc))
print("")

# Test the new classes on the protonet
print("Testing Model (test set size: {})".format(
    len(net.TestDataloader.dataset.all_items)))
avr_test_acc = net.Test();
print("Avr test acc: {:0.02f}".format(avr_test_acc))
