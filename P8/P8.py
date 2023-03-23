import os

from DataConverters.DirectDataConverter import DirectDataConverter
from DataConverters.ShapeletHistogramConverter import ShapeletHistogramConverter
from DataConverters.DataConverterOptions import DataConverterOptions
from DataConverters.ShapeletHistogramConverter.ShapeletHistogramConverterOptions import ShapeletHistogramConverterOptions
from NetTrainers.ProtoNetTrainer import NetTrainer
from NetTrainers.ProtoNetTrainer import NetOptions
from Datasets.UCR import UCR
from Datasets import DatasetBuilder

targetDataDir = "formated" + os.sep + "swedishLeaf" + os.sep
targetOutputDir = "output" + os.sep + "swedishLeaf"

# Convert data into a new format
dataOptions = ShapeletHistogramConverterOptions()
dataOptions.FormatedFolder = targetDataDir
dataOptions.TrainValSplit = 0;
dataOptions.TestClassesSplit = 5;
dataOptions.SourceTrainData = "./Data/SwedishLeaf/SwedishLeaf_TRAIN.tsv";
dataOptions.SourceTestData = "./Data/SwedishLeaf/SwedishLeaf_TEST.tsv";
dataOptions.depth = 40;
dataOptions.maxWindowSize = 32;

#dataConverter = DirectDataConverter(dataOptions);
dataConverter = ShapeletHistogramConverter(dataOptions);
dataConverter.ConvertData()

# Setup protonet 
options = NetOptions.NetOptions
options.dataset_root = targetDataDir
options.experiment_root = targetOutputDir;
options.classes_per_it_tr = 3
options.num_query_tr = 2;
options.num_support_tr = 1;
options.classes_per_it_test = 3
options.train_epochs = 5;
options.test_epochs = 3;
options.load_val_set = False;
options.dataset_name = "UCR"

net = NetTrainer.NetTrainer(options, DatasetBuilder.GetDataset(options.dataset_name))

# Train the protonet
print("Training Model")
best_train_acc = net.Train();
print("Best train acc: {:0.02f}".format(best_train_acc))
print("")

# Test the new classes on the protonet
print("Testing Model")
avr_test_acc = net.Test();
print("Avr test acc: {:0.02f}".format(avr_test_acc))
