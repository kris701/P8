import os
import pandas as pd
import matplotlib.pyplot as plt
from .BaseVisualiser import BaseVisualiser

class ShapeletHistogramVisualiser(BaseVisualiser):
    def __init__(self, datasetPath: str) -> None:
        super().__init__(datasetPath)

    def VisualizeClass(self, classIndex, visualizeTrain : bool = True, visualizeTest : bool = True) -> plt.figure:
        classData = self._GetClassData(visualizeTrain, visualizeTest);
        shapeletData = self.LoadSingleDataDir(os.path.join(self.DatasetPath, "features", "shapelets"))
        featureData = self._GetFeatureData();

        fig = plt.figure(figsize=self.GraphSize)
        gs = fig.add_gridspec(3, len(shapeletData))

        fig.suptitle("Class id: " + str(classIndex) + "(Train: " + str(visualizeTrain) + ", Test: " + str(visualizeTest) + ")")

        index : int = 0
        for key in shapeletData:
            shapeletAxis = fig.add_subplot(gs[2, index])
            shapeletAxis.set_title(str(featureData["Attribute"][key]));
            shapeletAxis.set_yticks([])
            shapeletAxis.plot(shapeletData[key])
            index += 1
            
        transformed = {}
        for key in shapeletData:
            transformed[key] = []
            index = int(key);
            for sample in classData[classIndex]:
                transformed[key].append(sample[index])

        sample_axis = fig.add_subplot(gs[:2, :])
        sample_axis.boxplot(transformed.values(), labels=transformed.keys())

        return fig

    def VisualizeAllClasses(self, visualizeTrain : bool = True, visualizeTest : bool = True) -> plt.figure:
        classData = self._GetClassData(visualizeTrain, visualizeTest);
        
        rows, cols = self.GetPlotSize(len(classData));
        fig, ax = plt.subplots(nrows=rows, ncols=cols, sharex=True, sharey=True, figsize=self.GraphSize)
        fig.suptitle("All Classes (Train: " + str(visualizeTrain) + ", Test: " + str(visualizeTest) + ")")
        colIndex : int = 0;
        rowIndex : int = 0;
        for classIndex in classData:
            transformed = {}
            for key in range(0, len(classData[classIndex][0])):
                transformed[key] = []
                index = int(key);
                for sample in classData[classIndex]:
                    transformed[key].append(sample[index])
            
            ax[rowIndex, colIndex].title.set_text("Id: " + str(classIndex))
            ax[rowIndex, colIndex].boxplot(transformed.values(), labels=transformed.keys())

            colIndex += 1;
            if colIndex >= cols:
                colIndex = 0;
                rowIndex += 1;
        return fig;

    def _GetClassData(self, getTrain : bool = True, getTest : bool = True) -> dict:
        data = {};
        if getTrain:
            self._LoadDataIntoDict(data, "train.txt");
        if getTest:
            self._LoadDataIntoDict(data, "test.txt");
        return data;

    def _LoadDataIntoDict(self, data : dict, splitFile : str) -> None:
        splitFilesPath = os.path.join(self.DatasetPath, "split", splitFile);
        with open(splitFilesPath, "r") as splitFiles:
            for file in splitFiles:
                file = file.replace("\n","")
                classID = int(file.split("/")[1])
                file = os.path.join(self.DatasetPath, file)
                fileData = []
                with open(file, "r") as dataFile:
                    for datapoint in dataFile:
                        value = float(datapoint.replace("\n",""))
                        fileData.append(value)
                if classID not in data:
                    data[classID] = []
                data[classID].append(fileData)

    def _GetFeatureData(self) -> dict:
        featureData = {};
        featureDataCsv = os.path.join(self.DatasetPath, "features", "features.csv");
        csvData = pd.read_csv(featureDataCsv);
        featureData = csvData.to_dict();
        return featureData