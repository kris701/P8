import os
import pandas as pd

def load_and_format_data(fp):
    df = pd.read_csv(fp, delimiter="\t", header=None)
    X = df.iloc[:, 1:]
    y = df.iloc[:, 0]
    classes = []
    for c in range(len(set(y))):
        classes.append([X.iloc[i] for i in range(len(y)) if y[i] == c])
    return classes

def write_data(classes, destination_folder):
    os.makedirs(destination_folder)
    for j, c in enumerate(classes):
        os.makedirs(destination_folder + "\\" + str(j))
        for i, item in enumerate(c):
            item.to_csv(path_or_buf=destination_folder + "\\" + str(j) + "\\" + str(i) + ".csv", sep=',', header=False)
