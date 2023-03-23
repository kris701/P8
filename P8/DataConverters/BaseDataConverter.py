import os
import shutil
import hashlib
import json

from .DataConverterOptions import DataConverterOptions

class BaseDataConverter(object):
    Options : DataConverterOptions = None;
    
    def __init__(self, options : DataConverterOptions) -> None:
        self.Options = options

    def ConvertData(self):
        raise Exception("Not Implemented!")

    def HaveConvertedBefore(self):
        if not os.path.isfile(self.Options.FormatedFolder + os.sep + "checksum.txt"):
            return False;
        this_checksum = self.GetChecksum();
        file_checksum = "";
        with open(self.Options.FormatedFolder + os.sep + "checksum.txt", "r") as f:
            file_checksum = f.readline()
        return file_checksum.replace("\n","") == this_checksum

    def PurgeOutputFolder(self):
        print("Purging old dataset")
        if os.path.isdir(self.Options.FormatedFolder):
            shutil.rmtree(self.Options.FormatedFolder);

    def OutputChecksum(self):
        checksum = self.GetChecksum();
        with open(self.Options.FormatedFolder + os.sep + "checksum.txt", "w") as f:
            f.write(checksum + "\n")
    
    def GetChecksum(self):
        data = json.dumps(self.Options.__dict__, sort_keys=True).encode('utf-8')
        return hashlib.md5(data).hexdigest()


