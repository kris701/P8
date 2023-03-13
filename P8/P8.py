from DataConverters.DataConverter import DataWriter as dw
from ProtoNets.BaseProtonet import BaseProtoNet as bpn
from Datasets.SwedishLeaf import SwedishLeafDataset

dataWriter = dw();
res = dataWriter.Convert("./Data/SwedishLeaf/SwedishLeaf_TRAIN.tsv", "./Data/SwedishLeaf/SwedishLeaf_TEST.tsv", 0.2)

net = bpn()
net.main(SwedishLeafDataset);

