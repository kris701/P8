import matplotlib.pyplot as plt
import numpy as np
from .BaseVisualiser import BaseVisualiser

class ResultsVisualiser(BaseVisualiser):
    def __init__(self, datasetPath: str) -> None:
        super().__init__(datasetPath)

    def VisualiseAccuracy(self, data : dict, title : str) -> plt.figure:
        datasetTotal = []
        for key in data:
            for key2 in data[key]:
                if key2 not in datasetTotal:
                    datasetTotal.append(key2);

        showData = {}
        for dataItem in data:
            showData[dataItem] = []
            for dataset in datasetTotal:
                if dataset in data[dataItem]:
                    showData[dataItem].append(data[dataItem][dataset]);
                else:
                    showData[dataItem].append(0)

        width = 0.1  # the width of the bars
        x = np.arange(len(datasetTotal))
        fig, ax = plt.subplots(figsize=self.GraphSize)
        fig.suptitle(title);
        index = 0
        for datapoint in showData:
            offset = -((len(showData) * width) / 2) + (index * width)
            rects = ax.bar(x + offset, showData[datapoint], width, label=datapoint)
            ax.bar_label(rects, padding=3)
            index += 1

        ax.set_xticks(x, datasetTotal)
        ax.set_xticklabels(datasetTotal, rotation=45)
        ax.set_xlabel('Dataset')
        ax.set_ylim(0, 1)
        ax.set_ylabel('Accuracy')
        ax.legend(loc='right')
        plt.xlabel("Method")
        plt.ylabel("Accuracy")
        return fig
    
    def VisualiseAverageAccuracy(self, data : dict, title : str) -> plt.figure:
        showData = {}
        for dataItem in data:
            showData[dataItem] = []
            for dataset in data[dataItem]:
                showData[dataItem].append(data[dataItem][dataset])

        fig = plt.figure(figsize=self.GraphSize)
        fig.suptitle(title);
        plt.boxplot(showData.values())
        plt.xticks(range(1, len(showData.keys()) + 1), showData.keys())
        plt.xlabel("Method")
        plt.ylabel("Accuracy")
        return fig
