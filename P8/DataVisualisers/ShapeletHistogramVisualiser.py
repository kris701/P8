import os
import matplotlib.pyplot as plt
import pandas as pd
from math import sqrt

class ShapeletHistogramVisualiser():
    DatasetPath : str = "";

    def __init__(self, datasetPath : str) -> None:
        self.DatasetPath = datasetPath.replace("/",os.sep).replace("\\",os.sep)

    def VisualizeClass(self, classIndex) -> plt.figure:
        classData = self._GetClassData();
        shapeletData = self._GetShapeletData();
        featureData = self._GetFeatureData();

        fig = plt.figure(constrained_layout=True)
        gs = fig.add_gridspec(3, len(shapeletData))

        fig.suptitle("Class id: " + str(classIndex))

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

    def VisualizeAllClasses(self) -> plt.figure:
        classData = self._GetClassData();
        
        rows, cols = self._GetPlotSize(len(classData));
        fig, ax = plt.subplots(nrows=rows, ncols=cols, sharex=True, sharey=True)
        colIndex : int = 0;
        rowIndex : int = 0;
        for classIndex in classData:
            transformed = {}
            for key in range(0, len(classData[classIndex][0])):
                transformed[key] = []
                index = int(key);
                for sample in classData[classIndex]:
                    transformed[key].append(sample[index])
            
            ax[rowIndex, colIndex].title.set_text("Class id: " + str(classIndex))
            ax[rowIndex, colIndex].boxplot(transformed.values(), labels=transformed.keys())

            colIndex += 1;
            if colIndex >= cols:
                colIndex = 0;
                rowIndex += 1;
        return fig;

    def VisualiseShapelets(self) -> plt.figure:
        shapeletData = self._GetShapeletData();
        
        rows, cols = self._GetPlotSize(len(shapeletData));
        fig, ax = plt.subplots(nrows=rows, ncols=cols, sharex=True, sharey=True)
        colIndex : int = 0;
        rowIndex : int = 0;
        for shapeletIndex in shapeletData:           
            ax[rowIndex, colIndex].title.set_text("Shapelet id: " + str(shapeletIndex))
            ax[rowIndex, colIndex].plot(shapeletData[shapeletIndex])

            colIndex += 1;
            if colIndex >= cols:
                colIndex = 0;
                rowIndex += 1;
        return fig;

    def VisualiseSourceData(self) -> plt.figure:
        sourceData = self._GetSourceData();

        rows, cols = self._GetPlotSize(len(sourceData));
        fig, ax = plt.subplots(nrows=rows, ncols=cols, sharex=True, sharey=True)
        colIndex : int = 0;
        rowIndex : int = 0;
        for classIndex in sourceData:
            transformed = {}
            for key in range(0, len(sourceData[classIndex][0])):
                transformed[key] = []
                index = int(key);
                for sample in sourceData[classIndex]:
                    transformed[key].append(sample[index])
            
            ax[rowIndex, colIndex].title.set_text("Class id: " + str(classIndex))
            ax[rowIndex, colIndex].plot(transformed.values())

            colIndex += 1;
            if colIndex >= cols:
                colIndex = 0;
                rowIndex += 1;
        return fig;

    def _GetClassData(self) -> dict:
        data = {};
        dataDir = os.path.join(self.DatasetPath, "data");
        for className in os.listdir(dataDir):
            classData = []
            for fileName in os.listdir(os.path.join(dataDir, className)):
                fileData = []
                with open(os.path.join(dataDir, className, fileName), "r") as file:
                    for line in file:
                        value = float(line.replace("\n",""))
                        fileData.append(value)
                classData.append(fileData)
            data[int(className)] = classData
        return data;

    def _GetShapeletData(self) -> dict:
        shapeletData = {};
        shapeletDataDir = os.path.join(self.DatasetPath, "features", "shapelets");
        for shapeletName in os.listdir(shapeletDataDir):
            fileData = []
            with open(os.path.join(shapeletDataDir, shapeletName), "r") as file:
                for line in file:
                    value = float(line.replace("\n",""))
                    fileData.append(value)
            shapeletData[int(shapeletName)] = fileData
        return shapeletData

    def _GetSourceData(self) -> dict:
        data = {};
        dataDir = os.path.join(self.DatasetPath, "source");
        for className in os.listdir(dataDir):
            classData = []
            for fileName in os.listdir(os.path.join(dataDir, className)):
                fileData = []
                with open(os.path.join(dataDir, className, fileName), "r") as file:
                    for line in file:
                        value = float(line.replace("\n",""))
                        fileData.append(value)
                classData.append(fileData)
            data[int(className)] = classData
        return data;

    def _GetFeatureData(self) -> dict:
        featureData = {};
        featureDataCsv = os.path.join(self.DatasetPath, "features", "features.csv");
        csvData = pd.read_csv(featureDataCsv);
        featureData = csvData.to_dict();
        return featureData
    
    def _GetPlotSize(self, nClasses) -> tuple[int,int]:
        for n in range (2, 99999):
            if nClasses <= n * n:
                return n,n
