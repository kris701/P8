import configparser
import os
from os.path import isfile

class DataConverterOptions():
    def __init__(self, configName) -> None:
        config = configparser.RawConfigParser()
        config.optionxform = lambda option: option
        config.read(configName)
        for index in config["DATACONVERTER"]:
            if index in self.__annotations__:
                typeName = self.__annotations__[index].__name__;
                if typeName == "str":
                    self.__dict__[index] = config["DATACONVERTER"][index]
                elif typeName == "bool":
                    self.__dict__[index] = config["DATACONVERTER"].getboolean(index)
                elif typeName == "int":
                    self.__dict__[index] = config["DATACONVERTER"].getint(index)
                elif typeName == "float":
                    self.__dict__[index] = config["DATACONVERTER"].getfloat(index)
                else:
                    raise Exception("Invalid config type!")

    # What converter to use
    UseConverter : str = "DirectDataConverter"
    # Source datasets to convert from
    SourceTrainData : str = "";
    SourceTestData : str = "";
    # Output formated data to
    FormatedFolder : str = "Formatted/BaseOutput";

    # What percentage of the data classes should be put into the test data
    TestClassesSplit : float = 0.2;
    # What percentage of the train data, should be regarded as a Validation set.
    TrainValSplit : float = 0.7;

    # Settings for the ShapeletHistogramConverter
    minWindowSize : int = 2;
    maxWindowSize : int = 4;
    depth : int = 1;

    def VerifySettings(self):
        if not os.path.isfile(self.SourceTrainData): raise Exception("Source train dataset not found: '" + self.SourceTrainData + "'")
        if not os.path.isfile(self.SourceTestData): raise Exception("Source test dataset not found: '" + self.SourceTestData + "'")




