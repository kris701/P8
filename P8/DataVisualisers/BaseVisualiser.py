import os
import matplotlib.pyplot as plt

class BaseVisualiser():
    DatasetPath : str = "";
    GraphSize = (20,10);

    def __init__(self, datasetPath : str) -> None:
        self.DatasetPath = datasetPath.replace("/",os.sep).replace("\\",os.sep)

    def GetPlotSize(self, nClasses) -> tuple[int,int]:
        for n in range (2, 99999):
            if nClasses <= n * n:
                return n,n

    def VisualizeDictionary(self, dictValues : dict, title : str, labelPrefix : str = "", xLabel : str = "", yLabel : str = "") -> plt.figure:
        fig = plt.figure(figsize=self.GraphSize)
        fig.suptitle(title);
        xLabels = []
        for key in dictValues.keys():
            xLabels.append(labelPrefix + str(key))
        plt.bar(xLabels, dictValues.values())
        plt.xlabel(xLabel)
        plt.ylabel(yLabel)
        return fig;

    def SaveAndClose(self, fig : plt.figure, path : str):
        fig.savefig(path)
        plt.close(fig)

    def VisualiseIndividualDatapoints(self, path : str, title : str) -> plt.figure:
        data = self.LoadSingleDataDir(path);
        
        rows, cols = self.GetPlotSize(len(data));
        fig, ax = plt.subplots(nrows=rows, ncols=cols, sharex=True, sharey=True, figsize=self.GraphSize)
        fig.suptitle(title)
        colIndex : int = 0;
        rowIndex : int = 0;
        for index in data:           
            ax[rowIndex, colIndex].title.set_text("Id: " + str(index))
            ax[rowIndex, colIndex].plot(data[index])

            colIndex += 1;
            if colIndex >= cols:
                colIndex = 0;
                rowIndex += 1;
        return fig;

    def VisualizeCombinedDatapoints(self, path : str, title : str) -> plt.figure:
        data = self.LoadDataDir(path);

        rows, cols = self.GetPlotSize(len(data));
        fig, ax = plt.subplots(nrows=rows, ncols=cols, sharex=True, sharey=True, figsize=self.GraphSize)
        fig.suptitle(title)
        colIndex : int = 0;
        rowIndex : int = 0;
        for index in data:
            transformed = {}
            for key in range(0, len(data[index][0])):
                transformed[key] = []
                listIndex = int(key);
                for sample in data[index]:
                    transformed[key].append(sample[listIndex])
            
            ax[rowIndex, colIndex].title.set_text("Id: " + str(index) + " (n: " + str(len(data[index])) + ")")
            ax[rowIndex, colIndex].plot(transformed.values())

            colIndex += 1;
            if colIndex >= cols:
                colIndex = 0;
                rowIndex += 1;
        return fig;

    def LoadDataDir(self, path : str) -> dict:
        if not os.path.exists(path):
            raise FileNotFoundError("Error, the folder '" + path + "' was not found!")

        data = {};
        for folderID in os.listdir(path):
            folderName = os.path.join(path, folderID);
            if os.path.isdir(folderName):
                subFolderData = []
                for fileID in os.listdir(folderName):
                    fileData = []
                    with open(os.path.join(path, folderID, fileID), "r") as file:
                        for line in file:
                            value = float(line.replace("\n",""))
                            fileData.append(value)
                    subFolderData.append(fileData)
                data[int(folderID)] = subFolderData
        return data;

    def LoadSingleDataDir(self, path : str) -> dict:
        if not os.path.exists(path):
            raise FileNotFoundError("Error, the folder '" + path + "' was not found!")

        data = {};
        if os.path.isdir(path):
            for fileID in os.listdir(path):
                fileData = []
                with open(os.path.join(path, fileID), "r") as file:
                    for line in file:
                        value = float(line.replace("\n",""))
                        fileData.append(value)
                data[int(fileID)] = fileData;
        return data;
