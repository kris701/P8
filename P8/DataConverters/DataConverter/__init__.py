import os
from tokenize import Double
import pandas as pd
import random
import math
import shutil

class DataWriter():
    def _load_data(self, fp): #load the data and sort into a list of classes, each class containing a list of dataframes, each of which is a time series
        df = pd.read_csv(fp, delimiter="\t", header=None)
        X = df.iloc[:, 1:]
        y = df.iloc[:, 0]
        classes = []
        for c in set(y):
            classes.append([X.iloc[i] for i in range(len(y)) if y[i] == c])
        return classes

    def _write_data(self, classes, dataset_root): #write data to the data set folder. 
        destination_folder = dataset_root + os.sep + "data"
        if not os.path.isdir(destination_folder):
            os.makedirs(destination_folder)
        for j, c in enumerate(classes):
            os.makedirs(destination_folder + os.sep + str(j+1))
            for i, item in enumerate(c):
                item.to_csv(path_or_buf=destination_folder + os.sep + str(j+1) + os.sep + str(i) + ".csv", sep=',', header=False, index=False)

    def _split_trainval(self, data, percentage_train):
        random.shuffle(data)
        split = math.floor(len(data) * percentage_train)
        train = data[:split]
        val = data[split:]
        return train, val

    def _create_splits(self, dataset_root, n_trainval_classes):
        data_folder = "data"
        if not os.path.isdir(dataset_root + os.sep + data_folder):
            os.makedirs(dataset_root + os.sep + data_folder)
        class_folders = [data_folder + os.sep + fldr for fldr in os.listdir(dataset_root + os.sep + data_folder)]
        trainval_folders = class_folders[:n_trainval_classes]
        test_folders = class_folders[n_trainval_classes:]
        trainval_files = []

        trainval_files = self._get_filepaths(dataset_root, trainval_folders)
        train, val = self._split_trainval(trainval_files, 0.8)
        test_files = self._get_filepaths(dataset_root, test_folders)
        random.shuffle(test_files)
        split_path = dataset_root + os.sep + "split"
        os.mkdir(split_path)
    
        self._writer("train.txt", train, split_path)
        self._writer("val.txt", val, split_path)
        self._writer("test.txt", test_files, split_path)

    def _writer(self, fname, paths, split_path):
        with open(split_path + os.sep + fname, "w") as f:
            f.write("\n".join(paths))

    def _get_filepaths(self, dataset_root, folders):
        result = []
        for fldr in folders:
            for file in os.listdir(dataset_root + os.sep + os.sep + fldr):
                result.append(fldr + os.sep + file)
        return result

    def _formatData(self, trainName : str, testName : str): # load and write the swedishleaf data set in the format that our protonet likes
        train_data = self._load_data(trainName)
        test_data = self._load_data(testName)
        return [train_data[i] + test_data[i] for i in range(len(train_data))]

    def Convert(self, trainName : str, testName : str, testClassesPercent : float):
        if not os.path.isdir("formated"):
            print("Formating dataset...")
            formated = self._formatData(trainName, testName)
            self._write_data(formated, "formated")
            self._create_splits("formated", int(len(formated) * (1 - testClassesPercent)))
        else:
            print("Dataset already formated!")



