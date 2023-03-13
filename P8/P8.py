from DataConverters.DataConverter import DataWriter as dw
from ProtoNets.BaseProtonet import train as tr
from Datasets.SwedishLeaf import SwedishLeafDataset

dataWriter = dw();
res = dataWriter.Convert("./Data/SwedishLeaf/SwedishLeaf_TRAIN.tsv", "./Data/SwedishLeaf/SwedishLeaf_TEST.tsv")

tr.main(SwedishLeafDataset);

