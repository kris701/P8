from ..BaseNetTrainer import BaseNetTrainer
from ..NetOptions import NetOptions

import torch.utils.data as data
import os
import pathlib
import subprocess
import checksumdir
import hashlib
import json

class NetTrainer(BaseNetTrainer):
    compile_dir : str = "./NaiveKNN";

    def __init__(self, options: NetOptions, dataset: data.Dataset) -> None:
        super().__init__(options, dataset)

    def Train(self) -> float:
        print("No training needed for KNN")
        return -1;

    def Test(self) -> float:
        workingDir = pathlib.Path().resolve();
        thisFile = pathlib.Path(__file__).parent.resolve();

        if self._ShouldRecompile():
            print("Compiling the naive KNN...")
            self._CompileFeatureExtractor(os.path.join(thisFile, self.compile_dir))

        extension = "";
        if os.name == "nt":
            extension = ".exe"
        executable = os.path.join(thisFile, "NaiveKNN/out/Release/Classifier" + extension)
        res = subprocess.run([executable, 
                        "--data", str(os.path.join(workingDir, self.Options.dataset_root.replace("./","").replace("/",os.sep))),
                        "--neighbors", str(self.Options.KNN_Neighbors)
                        ]) 
        if (res.returncode > 100):
            raise Exception("Something went wrong with the KNN!");

        return res.returncode / 100;

    def _CompileFeatureExtractor(self, compileDir):
        if os.name == "nt":
            subprocess.run(["cmake", compileDir, "-B " + os.path.join(compileDir, "out"), "-DCMAKE_BUILD_TYPE=RELEASE"]) 
        else:
            subprocess.run(["cmake", compileDir, "-B " + os.path.join(compileDir, "out"), "-DCMAKE_BUILD_TYPE=RELEASE", "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=Release"]) 
        subprocess.run(["cmake", "--build", os.path.join(compileDir, "out"), "--config Release"]) 

        checksum_value = self._GetExtractorChecksum();
        thisFile = pathlib.Path(__file__).parent.resolve();
        if not os.path.isdir(os.path.join(thisFile, "checksum")):
            os.mkdir(os.path.join(thisFile, "checksum"))
        with open(os.path.join(thisFile, "checksum" + os.sep + "checksum.txt"), "w") as f:
            f.write(checksum_value + "\n")
    
    def _ShouldRecompile(self):
        thisFile = pathlib.Path(__file__).parent.resolve();
        if not os.path.isdir(os.path.join(thisFile, self.compile_dir, "out")):
            return True;
        if not os.path.isfile(os.path.join(thisFile, "checksum" + os.sep + "checksum.txt")):
            return True;
        thisFile = pathlib.Path(__file__).parent.resolve();
        current_checksum = checksumdir.dirhash(os.path.join(thisFile, "NaiveKNN"));
        saved_checksum = "";
        with open(os.path.join(thisFile, "checksum" + os.sep + "checksum.txt"), "r") as f:
            saved_checksum = f.readline()
        return current_checksum != saved_checksum.replace("\n","");

    def _GetExtractorChecksum(self):
        thisFile = pathlib.Path(__file__).parent.resolve();
        return checksumdir.dirhash(os.path.join(thisFile, "NaiveKNN"))
