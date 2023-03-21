class DataConverterOptions():
    # Source datasets to convert from
    SourceTrainData : str = "";
    SourceTestData : str = "";
    # Output formated data to
    FormatedFolder : str = "formated/BaseOutput";

    # What percentage of the data classes should be put into the test data
    TestClassesSplit : float = 0.2;
    # What percentage of the train data, should be regarded as a Validation set.
    TrainValSplit : float = 0.7;




