import os
import pathlib
import subprocess

from .ShapeletHistogramConverterOptions import ShapeletHistogramConverterOptions
from ..BaseDataConverter import BaseDataConverter

class ShapeletHistogramConverter(BaseDataConverter):
    compile_dir : str = "./FeatureExtraction";

    def __init__(self, options: ShapeletHistogramConverterOptions) -> None:
        super().__init__(options)

    def ConvertData(self):
        if not os.path.isdir(self.Options.FormatedFolder):

            workingDir = pathlib.Path().resolve();
            thisFile = pathlib.Path(__file__).parent.resolve();
            if not os.path.isdir(os.path.join(thisFile, self.compile_dir, "out")):
                print("Compiling the feature extractor...")
                self._CompileFeatureExtractor(os.path.join(thisFile, self.compile_dir))

            print("Formating dataset. This may take a while...")
            
            extension = "";
            if os.name == "nt":
                extension = ".exe"
            executable = os.path.join(thisFile, "FeatureExtraction/out/Release/FeatureExtraction" + extension)
            subprocess.run([executable, 
                            "--train", str(os.path.join(workingDir, self.Options.SourceTrainData.replace("./","").replace("/",os.sep))),
                            "--test", str(os.path.join(workingDir, self.Options.SourceTestData.replace("./","").replace("/",os.sep))),
                            "--out", str(os.path.join(workingDir, self.Options.FormatedFolder.replace("./","").replace("/",os.sep))),
                            "--split", str(self.Options.TrainValSplit),
                            "--valtrainsplit", str(self.Options.TestClassesSplit),
                            "--depth", str(self.Options.depth),
                            "--minWindowSize", str(self.Options.minWindowSize),
                            "--maxWindowSize", str(self.Options.minWindowSize),
                            ]) 

            print("Formating complete!")
        else:
            print("Dataset already formated!")

    def _CompileFeatureExtractor(self, compileDir):
        subprocess.run(["cmake", compileDir, "-B " + os.path.join(compileDir, "out")]) 
        subprocess.run(["cmake", "--build", os.path.join(compileDir, "out"), "--config Release"]) 
        pass;