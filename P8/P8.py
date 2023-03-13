from DataConverters.DataConverter import DataWriter as dw
from ProtoNets.BaseProtonet import BaseProtoNet as bpn
from ProtoNets.BaseProtonet import ProtoNetOptions as bpnOpt
from Datasets.SwedishLeaf import SwedishLeafDataset

dataWriter = dw();
res = dataWriter.Convert("./Data/SwedishLeaf/SwedishLeaf_TRAIN.tsv", "./Data/SwedishLeaf/SwedishLeaf_TEST.tsv", 0.2)

options = bpnOpt()
options.classes_per_it_tr = 5
options.classes_per_it_val = 5
options.epochs = 10;

net = bpn()
net.Run(SwedishLeafDataset, options);

