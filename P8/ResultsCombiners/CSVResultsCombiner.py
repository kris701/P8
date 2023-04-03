import pandas as pd

class CSVResultsCombiner():
    def Combine(self, fileSources : list, dictSources : list, dictExclusiveCombine : bool = False) -> dict:
        fullResults = {}
        acceptedDatasets = []

        for item in dictSources:
            for key in item:
                fullResults[key] = item[key]
                for key2 in item[key]:
                    acceptedDatasets.append(key2);

        for source in fileSources:
            csvData = pd.read_csv(source).to_dict();
            datasetIDs = csvData["datasetName"];

            for setValue in csvData:
                if setValue != "datasetName" and setValue != "NumberOfClasses":
                    if dictExclusiveCombine is False:
                        if setValue not in fullResults:
                            fullResults[setValue] = {}
                        for datapoint in csvData[setValue]:
                            fullResults[setValue][datasetIDs[datapoint]] = csvData[setValue][datapoint]
                    else:
                        if setValue not in fullResults:
                            fullResults[setValue] = {}
                        for datapoint in csvData[setValue]:
                            if datasetIDs[datapoint] in acceptedDatasets:
                                fullResults[setValue][datasetIDs[datapoint]] = csvData[setValue][datapoint]


        return fullResults
