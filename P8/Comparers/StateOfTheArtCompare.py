import pandas as pd

class StateOfTheArtCompare():
    def GetFullComparisons(self, fileSources : list, dictSources : list) -> dict:
        fullResults = {}

        for source in fileSources:
            csvData = pd.read_csv(source).to_dict();
            datasetIDs = csvData["datasetName"];

            for setValue in csvData:
                if setValue != "datasetName" and setValue != "NumberOfClasses":
                    if setValue not in fullResults:
                        fullResults[setValue] = {}
                    for datapoint in csvData[setValue]:
                        fullResults[setValue][datasetIDs[datapoint]] = csvData[setValue][datapoint]

        for item in dictSources:
            for key in item:
                fullResults[key] = item[key]

        return fullResults
