import os
import pathlib
import subprocess
import shutil
import checksumdir
import hashlib
import json
    
from ..DataConverterOptions import DataConverterOptions
from ..BaseDataConverter import BaseDataConverter

class ShapeletHistogramConverter(BaseDataConverter):
    compile_dir : str = "./FeatureExtraction";

    def __init__(self, options: DataConverterOptions, debugMode : bool = False) -> None:
        super().__init__(options, debugMode)
        thisFile = pathlib.Path(__file__).parent.resolve();
        self.compile_dir = os.path.join(thisFile, self.compile_dir)

    def ConvertData(self):
        if not self.HaveConvertedBefore():
            self.PurgeOutputFolder();

            workingDir = pathlib.Path().resolve();
            thisFile = pathlib.Path(__file__).parent.resolve();
            if self._ShouldRecompile():
                if self.DebugMode is True:
                    print("Compiling the feature extractor...")
                self._CompileFeatureExtractor()

            if self.DebugMode is True:
                print("Formating dataset. This may take a while...")
            
            extension = "";
            if os.name == "nt":
                extension = ".exe"
            executable = os.path.join(thisFile, "FeatureExtraction/out/Release/FeatureExtraction" + extension)
            processParams = [executable, 
                            "--train", str(os.path.join(workingDir, self.Options.SourceTrainData.replace("./","").replace("/",os.sep))),
                            "--test", str(os.path.join(workingDir, self.Options.SourceTestData.replace("./","").replace("/",os.sep))),
                            "--out", str(os.path.join(workingDir, self.Options.FormatedFolder.replace("./","").replace("/",os.sep))),
                            "--split", str(self.Options.TestClassesSplit),
                            "--minWindowSize", str(self.Options.minWindowSize),
                            "--maxWindowSize", str(self.Options.maxWindowSize),
                            "--featureCount", str(self.Options.featureCount),
                            "--attributes", str(self.Options.attributes),
                            "--smoothingDegree", str(self.Options.smoothingDegree),
                            "--noisifyAmount", str(self.Options.noisifyAmount)
                            ];

            if self.Options.deleteOriginal:
                processParams.append("--deleteOriginal");
            if self.Options.purgeOddData:
                processParams.append("--purge");

            if self.DebugMode is True:
                subprocess.run(processParams)
            else:
                subprocess.run(processParams,
                               stdout=subprocess.DEVNULL,
                               stderr=subprocess.DEVNULL) 

            self.OutputChecksum();
            if self.DebugMode is True:
                print("Formating complete!")
        else:
            if self.DebugMode is True:
                print("Dataset already formated!")

    def _CompileFeatureExtractor(self):
        if os.name == "nt":
            subprocess.run(["cmake", self.compile_dir, "-B " + os.path.join(self.compile_dir, "out"), "-DCMAKE_BUILD_TYPE=RELEASE"]) 
        else:
            subprocess.run(["cmake", self.compile_dir, "-B " + os.path.join(self.compile_dir, "out"), "-DCMAKE_BUILD_TYPE=RELEASE", "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=Release"]) 
        subprocess.run(["cmake", "--build", os.path.join(self.compile_dir, "out"), "--config Release", "-j 100"])

        checksum_value = self._GetExtractorChecksum();
        thisFile = pathlib.Path(__file__).parent.resolve();
        if not os.path.isdir(os.path.join(thisFile, "checksum")):
            os.mkdir(os.path.join(thisFile, "checksum"))
        with open(os.path.join(thisFile, "checksum" + os.sep + "checksum.txt"), "w") as f:
            f.write(checksum_value + "\n")
    
    def _ShouldRecompile(self) -> bool:
        thisFile = pathlib.Path(__file__).parent.resolve();
        if not os.path.isdir(os.path.join(thisFile, self.compile_dir, "out")):
            return True;
        if not os.path.isfile(os.path.join(thisFile, "checksum" + os.sep + "checksum.txt")):
            return True;
        thisFile = pathlib.Path(__file__).parent.resolve();
        current_checksum = checksumdir.dirhash(os.path.join(thisFile, "FeatureExtraction"));
        saved_checksum = "";
        with open(os.path.join(thisFile, "checksum" + os.sep + "checksum.txt"), "r") as f:
            saved_checksum = f.readline()
        return current_checksum != saved_checksum.replace("\n","");

    def _GetExtractorChecksum(self):
        thisFile = pathlib.Path(__file__).parent.resolve();
        return checksumdir.dirhash(os.path.join(thisFile, "FeatureExtraction"))

    def GetChecksum(self):
        data = str(
            json.dumps(self.Options.__dict__, sort_keys=True) + self._GetExtractorChecksum()
            ).encode('utf-8')
        return hashlib.md5(data).hexdigest()