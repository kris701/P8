import os
from Helpers import CSVHelper
from Helpers import ComparisonDataHelper

class ResultsCombiner():
    def CombineDatasetsIn(self, path : str, folderMatch : str, outputFile : str) -> None:
        if os.path.exists(outputFile):
            os.remove(outputFile)

        allComparableCsvs = self.GetAllResultsConfigsInDir(path, folderMatch);
        combinedData = ComparisonDataHelper.CombineDictionaries(allComparableCsvs, []);
        header = ["datasetName", "NumberOfClasses"]
        datasets = []
        for key in combinedData:
            if key not in header:
                header.append(key);

            for dataset in combinedData[key]:
                if dataset not in datasets:
                    datasets.append(dataset)

        CSVHelper.AppendToCSV(header, outputFile);
        for dataset in datasets:
            line = []
            for key in combinedData:
                if dataset in combinedData[key]:
                    line.append(combinedData[key][dataset])
                else:
                    line.append(0)
            CSVHelper.AppendToCSV(line, outputFile);

    def GetAllResultsConfigsInDir(self, path : str, folderMatch : str) -> list:
        retItems = []
        for item in os.listdir(path):
            if folderMatch in item:
                for comparableCSV in os.listdir(os.path.join(path, item)):
                    if comparableCSV == "comparable.csv":
                        if os.path.isfile(os.path.join(path, item, comparableCSV)):
                            retItems.append(os.path.join(path, item, comparableCSV))
        return retItems