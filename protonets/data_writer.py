import os
import pandas as pd
import random
import math

def load_data(fp): #load the data and sort into a list of classes, each class containing a list of dataframes, each of which is a time series
    df = pd.read_csv(fp, delimiter="\t", header=None)
    X = df.iloc[:, 1:]
    y = df.iloc[:, 0]
    classes = []
    for c in set(y):
        classes.append([X.iloc[i] for i in range(len(y)) if y[i] == c])
    return classes

def write_data(classes, dataset_root): #write data to the data set folder. 
    destination_folder = dataset_root + os.sep + "data"
    if not os.path.isdir(destination_folder):
        os.makedirs(destination_folder)
    for j, c in enumerate(classes):
        os.makedirs(destination_folder + os.sep + str(j+1))
        for i, item in enumerate(c):
            item.to_csv(path_or_buf=destination_folder + os.sep + str(j+1) + os.sep + str(i) + ".csv", sep=',', header=False, index=False)

def split_trainval(data, percentage_train):
    random.shuffle(data)
    split = math.floor(len(data) * percentage_train)
    train = data[:split]
    val = data[split:]
    return train, val

def create_splits(dataset_root, n_trainval_classes):
    data_folder = "data"
    if not os.path.isdir(dataset_root + os.sep + data_folder):
        os.makedirs(dataset_root + os.sep + data_folder)
    class_folders = [data_folder + os.sep + fldr for fldr in os.listdir(dataset_root + os.sep + data_folder)]
    trainval_folders = class_folders[:n_trainval_classes]
    test_folders = class_folders[n_trainval_classes:]
    trainval_files = []

    def get_filepaths(folders):
        result = []
        for fldr in folders:
            for file in os.listdir(dataset_root + os.sep + os.sep + fldr):
                result.append(fldr + os.sep + file)
        return result

    trainval_files = get_filepaths(trainval_folders)
    train, val = split_trainval(trainval_files, 0.8)
    test_files = get_filepaths(test_folders)
    split_path = dataset_root + os.sep + "split"
    os.mkdir(split_path)
    
    def writer(fname, paths):
        with open(split_path + os.sep + fname, "w") as f:
            f.write("\n".join(paths))
    
    writer("train.txt", train)
    writer("val.txt", val)
    writer("trainval.txt", trainval_files)
    writer("test.txt", test_files)

def format_swedishleaf(): # load and write the swedishleaf data set in the format that our protonet likes
    train_data = load_data(SRC_FP)
    test_data = load_data(SRC_FP2)
    print("Train samples:", sum(len(train_data[i]) for i in range(len(train_data))))
    print("Test samples", sum(len(test_data[i]) for i in range(len(test_data))))
    return [train_data[i] + test_data[i] for i in range(len(train_data))]

