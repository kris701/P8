import os
import pandas as pd

#some paths you might find helpful:
TARGET_FP = "C:\\Users\\test\\Documents\\GitHub\\P8\\protonets\\prototypical-networks-master\\data\\SwedishLeaf\\data"
SRC_FP = "C:\\Users\\test\\Documents\\Data\\UCRArchive_2018\\SwedishLeaf\\SwedishLeaf_TRAIN.tsv"


def load_and_format_data(fp):
    df = pd.read_csv(fp, delimiter="\t", header=None)
    X = df.iloc[:, 1:]
    y = df.iloc[:, 0]
    classes = []
    for c in set(y):
        classes.append([X.iloc[i] for i in range(len(y)) if y[i] == c])
    return classes

def write_data(classes, destination_folder):
    os.makedirs(destination_folder)
    for j, c in enumerate(classes):
        os.makedirs(destination_folder + "\\" + str(j+1))
        for i, item in enumerate(c):
            item.to_csv(path_or_buf=destination_folder + "\\" + str(j+1) + "\\" + str(i) + ".csv", sep=',', header=False, index=False)