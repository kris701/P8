from DataConverters.DataConverter import DataWriter as dw
from ProtoNets.BaseProtonet import train as tr

#dataWriter = dw();
#res = dataWriter.formatData("./Data/SwedishLeaf/SwedishLeaf_TRAIN.tsv", "./Data/SwedishLeaf/SwedishLeaf_TEST.tsv")
#dataWriter.write_data(res, "./Datasets/SwedishLeaf_Formated");

tr.main();
