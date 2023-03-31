import os
import matplotlib.pyplot as plt
import pandas as pd
from math import sqrt

class ShapeletHistogramVisualiser():
    DatasetPath : str = "";

    def __init__(self, datasetPath : str) -> None:
        self.DatasetPath = datasetPath

    def VisualizeClass(self, classIndex):
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
            
        sample_axis = fig.add_subplot(gs[:2, :])
        for sample in classData[classIndex]:
            sample_axis.plot(sample)
        plt.show();

        pass

    def Visualize(self):
        classData = self._GetClassData();
        
        rows, cols = self._GetPlotSize(len(classData));
        fig, ax = plt.subplots(nrows=rows, ncols=cols, sharex=True, sharey=True)
        colIndex : int = 0;
        rowIndex : int = 0;
        for key in classData:
            ax[rowIndex, colIndex].title.set_text("Class id: " + str(key))
            for sample in classData[key]:
                ax[rowIndex, colIndex].plot(sample)
            colIndex += 1;
            if colIndex >= cols:
                colIndex = 0;
                rowIndex += 1;
        plt.show();

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

    def _GetFeatureData(self) -> dict:
        featureData = {};
        featureDataCsv = os.path.join(self.DatasetPath, "features", "features.csv");
        csvData = pd.read_csv(featureDataCsv);
        featureData = csvData.to_dict();
        return featureData

    # https://stackoverflow.com/questions/16907526/how-to-maximize-grid-dimensions-given-the-number-of-elements
    def _GetPlotSize(self, nClasses) -> tuple[int,int]:
        tempSqrt = sqrt(nClasses)
        divisors = []
        currentDiv = 1
        for currentDiv in range(nClasses):
            if nClasses % float(currentDiv + 1) == 0:
             divisors.append(currentDiv+1)

        hIndex = min(range(len(divisors)), key=lambda i: abs(divisors[i]-sqrt(nClasses)))
    
        if divisors[hIndex]*divisors[hIndex] == nClasses:
            return divisors[hIndex], divisors[hIndex]
        else:
            wIndex = hIndex + 1
            return divisors[hIndex], divisors[wIndex]
