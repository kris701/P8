import pandas as pd

class ComparisonDataHelper():
    def Combine(self, fileSources : list, dictSources : list, dictExclusiveCombine : bool = False) -> dict:
        fullResults = {}
        acceptedDatasets = []

        for item in dictSources:
            for key in item:
                fullResults[key] = {}
                for dataset in item[key]:
                    fullResults[key][dataset.lower()] = item[key][dataset]
                for key2 in item[key]:
                    acceptedDatasets.append(key2.lower());

        for source in fileSources:
            csvData = pd.read_csv(source).to_dict();
            datasetIDs = csvData["datasetName"];

            for setValue in csvData:
                if setValue != "datasetName" and setValue != "NumberOfClasses":
                    if dictExclusiveCombine is False:
                        if setValue not in fullResults:
                            fullResults[setValue] = {}
                        for datapoint in csvData[setValue]:
                            fullResults[setValue][datasetIDs[datapoint].lower()] = csvData[setValue][datapoint]
                    else:
                        if setValue not in fullResults:
                            fullResults[setValue] = {}
                        for datapoint in csvData[setValue]:
                            if datasetIDs[datapoint].lower() in acceptedDatasets:
                                fullResults[setValue][datasetIDs[datapoint].lower()] = csvData[setValue][datapoint]


        return fullResults
