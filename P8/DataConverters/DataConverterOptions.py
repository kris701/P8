import os

class DataConverterOptions():
    # What converter to use
    UseConverter : str = "DirectDataConverter"
    # Source datasets to convert from
    SourceTrainData : str = "";
    SourceTestData : str = "";
    # Output formated data to
    FormatedFolder : str = "Formatted/BaseOutput";

    # Number of shots
    TestClassesSplit : int = 5;

    # Settings for the ShapeletHistogramConverter
    minWindowSize : int = 2;
    maxWindowSize : int = 4;
    featureCount : int = 128;
    minSampleSize : int = 0;
    maxSampleSize : int = 5;
    attributes : str = "minDist";
    deleteOriginal : bool = False
    smoothingDegree : int = 0
    noisifyAmount : float = 0

    def VerifySettings(self):
        if not os.path.isfile(self.SourceTrainData): raise Exception("Source train dataset not found: '" + self.SourceTrainData + "'")
        if not os.path.isfile(self.SourceTestData): raise Exception("Source test dataset not found: '" + self.SourceTestData + "'")




