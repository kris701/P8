import configparser

class DataConverterOptions():
    def __init__(self) -> None:
        pass

    def __init__(self, configName) -> None:
        config = configparser.ConfigParser()
        config.read(configName)
        for index in config["OVERRIDES"]:
            typeName = self.__annotations__[index].__name__;
            if typeName == "str":
                self.__dict__[index] = config["OVERRIDES"][index]
            elif typeName == "bool":
                self.__dict__[index] = config["OVERRIDES"].getboolean(index)
            elif typeName == "int":
                self.__dict__[index] = config["OVERRIDES"].getint(index)
            elif typeName == "float":
                self.__dict__[index] = config["OVERRIDES"].getfloat(index)
            else:
                raise Exception("Invalid config type!")

    # What converter to use
    UseConverter : str = "DirectDataConverter"
    # Source datasets to convert from
    SourceTrainData : str = "";
    SourceTestData : str = "";
    # Output formated data to
    FormatedFolder : str = "formated/BaseOutput";

    # What percentage of the data classes should be put into the test data
    TestClassesSplit : float = 0.2;
    # What percentage of the train data, should be regarded as a Validation set.
    TrainValSplit : float = 0.7;




