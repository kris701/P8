import os
import matplotlib.pyplot as plt
import pandas as pd

class ShapeletHistogramVisualiser():
    DatasetPath : str = "";
    GraphSize = (10,10);

    def __init__(self, datasetPath : str) -> None:
        self.DatasetPath = datasetPath.replace("/",os.sep).replace("\\",os.sep)

    def VisualizeDictionary(self, dictValues : dict, title : str) -> plt.figure:
        fig = plt.figure(figsize=self.GraphSize)
        fig.suptitle(title);
        xLabels = []
        for key in dictValues.keys():
            xLabels.append("Class " + str(key))
        plt.bar(xLabels, dictValues.values())
        plt.xlabel("Class ID")
        plt.ylabel("Accuracy")
        return fig;

    def VisualizeClass(self, classIndex, visualizeTrain : bool = True, visualizeTest : bool = True) -> plt.figure:
        classData = self._GetClassData(visualizeTrain, visualizeTest);
        shapeletData = self._GetShapeletData();
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
        
        rows, cols = self._GetPlotSize(len(classData));
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

    def VisualiseShapelets(self) -> plt.figure:
        shapeletData = self._GetShapeletData();
        
        rows, cols = self._GetPlotSize(len(shapeletData));
        fig, ax = plt.subplots(nrows=rows, ncols=cols, sharex=True, sharey=True, figsize=self.GraphSize)
        fig.suptitle("Shapelets")
        colIndex : int = 0;
        rowIndex : int = 0;
        for shapeletIndex in shapeletData:           
            ax[rowIndex, colIndex].title.set_text("Id: " + str(shapeletIndex))
            ax[rowIndex, colIndex].plot(shapeletData[shapeletIndex])

            colIndex += 1;
            if colIndex >= cols:
                colIndex = 0;
                rowIndex += 1;
        return fig;

    def VisualiseSourceData(self) -> plt.figure:
        sourceData = self._GetSourceData();

        rows, cols = self._GetPlotSize(len(sourceData));
        fig, ax = plt.subplots(nrows=rows, ncols=cols, sharex=True, sharey=True, figsize=self.GraphSize)
        fig.suptitle("Source shots")
        colIndex : int = 0;
        rowIndex : int = 0;
        for classIndex in sourceData:
            transformed = {}
            for key in range(0, len(sourceData[classIndex][0])):
                transformed[key] = []
                index = int(key);
                for sample in sourceData[classIndex]:
                    transformed[key].append(sample[index])
            
            ax[rowIndex, colIndex].title.set_text("Id: " + str(classIndex))
            ax[rowIndex, colIndex].plot(transformed.values())

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
