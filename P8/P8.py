from DataConverters.DataConverter import DataWriter as dw
from NetTrainers.ProtoNetTrainer import ProtoNetTrainer as pnt
from NetTrainers.ProtoNetTrainer import ProtoNetOptions as pntOpt
from Datasets.SwedishLeaf import SwedishLeafDataset

dataWriter = dw();
res = dataWriter.Convert("./Data/SwedishLeaf/SwedishLeaf_TRAIN.tsv", "./Data/SwedishLeaf/SwedishLeaf_TEST.tsv", 0.2)

options = pntOpt()
options.classes_per_it_tr = 10
options.classes_per_it_val = 10
options.classes_per_it_test = 3
options.train_epochs = 5;
options.test_epochs = 1;

net = pnt(options)
bestModel = net.Train(SwedishLeafDataset);

