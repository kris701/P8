import os 
import pandas as pd

SRC_FP = os.path.join("C:\\", "Users", "Lenovo", "Datalogi", "SEM8", "P8_tests", "SwedishLeaf")

def load_data(fp):
    df = pd.read_csv(fp, delimiter="\t", header=None)
    X = df.iloc[:, 1:]
    y = df.iloc[:, 0]
    X = X.values.tolist()
    y = y.values.tolist()
    return X, y

def load_swedishleaf():
    pass

def generate_windows2(series: list, length: int):
    windows = []
    for i in range(len(series) - length + 1):
        windows.append(series[i:i+length])
    return windows

def generate_windows(series: list, min: int, max: int):
    print("---Generating Windows---")
    print("Total Series: ", len(series))
    windows = [] # List of windows
    for s in series:
        for l in range(min, max + 1):
            for w in generate_windows2(s, l):
                windows.append(w)
    print("Total Windows: ", len(windows))
    print("---Generating Windows---")
    return windows

tempSeries = [
    [1,2,3],
    [2,3,4],
    [2.0, 2.1, 3.4]
]

series, labels = load_data(SRC_FP + os.sep + "SwedishLeaf_TRAIN.tsv")

