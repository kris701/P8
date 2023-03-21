import cppyy
import os
import pathlib

from ..DataConverterOptions import DataConverterOptions
from ..BaseDataConverter import BaseDataConverter

class ShapeletHistogramConverter(BaseDataConverter):
    def __init__(self, options: DataConverterOptions) -> None:
        super().__init__(options)

    def _Initialize(self):
        # Include whatever cpp files are needed
        path = os.path.join(pathlib.Path(__file__).parent.resolve(), "FeatureExtraction" + os.sep + "main.cpp")
        cppyy.include(path)

    def ConvertData(self):
        if not os.path.isdir(self.Options.FormatedFolder):
            print("Compiling the feature extractor...")
            self._Initialize()
            print("Formating dataset. This may take a while...")

            options = cppyy.gbl.Arguments();
            workingDir = pathlib.Path().resolve();
            options.trainPath = str(os.path.join(workingDir, self.Options.SourceTrainData.replace("./","").replace("/",os.sep)));
            options.testPath = str(os.path.join(workingDir, self.Options.SourceTestData.replace("./","").replace("/",os.sep)));
            options.outPath = str(os.path.join(workingDir, self.Options.FormatedFolder.replace("./","").replace("/",os.sep)));
            options.valtrainsplit = self.Options.TrainValSplit;
            options.split = self.Options.TestClassesSplit

            cppyy.gbl.ConvertData(options);
            print("Formating complete!")
        else:
            print("Dataset already formated!")

