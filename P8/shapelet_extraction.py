import os
from math import log, e
import matplotlib.pyplot as plt

import numpy as np
import pandas as pd
from tqdm import tqdm

dirname = os.path.dirname(__file__)
filename = os.path.join(dirname, "data", "UCRArchive_2018", "SwedishLeaf")

# --- Data Loading ---

def load_and_format_data(fp): #load the data and sort into a list of classes, each class containing a list of dataframes, each of which is a time series
    df = pd.read_csv(fp, delimiter="\t", header=None)
    X = df.iloc[:, 1:]
    y = df.iloc[:, 0]
    classes = []
    for c in set(y):
        classes.append([X.iloc[i] for i in range(len(y)) if y[i] == c])
    result = []
    for c in classes:
        result.append([ts.tolist() for ts in c])
    return result

def load_data(fp):
    df = pd.read_csv(fp, delimiter="\t", header=None)
    X = df.iloc[:, 1:]
    y = df.iloc[:, 0]
    X = X.values.tolist()
    y = y.values.tolist()
    return X, y

# --- Windows ---

def generate_windows2(series: list, length: int):
    windows = []

    for i in range(len(series) - length + 1):
        offset = series[i]
        window = []
        for w in range(i, i + length):
            window.append(series[w] - offset)
        windows.append(window)
    return windows


def generate_windows(data: list, min: int, max: int):
    print("---Generating Windows---")
    windows = []
    windowCount = 0
    for class_series in data:
        windows.append([])
        for length in range(min, max + 1):
            for series in class_series:
                for w in generate_windows2(series, length):
                    windows[len(windows) - 1].append(w)
                    windowCount += 1
    print("Total Windows: ", windowCount)
    print("---Finished Generating Windows---")
    return windows

# --- Match Frequency ---

def is_match(s1: list, s2: list):
    offset = s1[0] - s2[0]

    tolerance = 0.1
    for i in range(1, len(s1)):
        if abs(s1[i] - s2[i] - offset) > tolerance:
            return False

    return True


def match_frequency(series: list, window: list):
    matches = 0
    total = 0
    for i in range(len(series) - len(window) + 1):
        if is_match(series[i:i+len(window)], window):
            matches += 1
        total += 1
    return matches / total


# --- Information Gain ---
def calculate_entropy_set(class_series: list):
    total = 0
    for cs in class_series:
        total += len(cs)

    inverse_entropy = 0

    for set in class_series:
        prob = len(set) / total
        inverse_entropy += prob * log(prob)

    return -inverse_entropy

# From: https://stackoverflow.com/questions/15450192/fastest-way-to-compute-entropy-in-python
def calculate_entropy(labels: list, base=None):
    """ Computes entropy of label distribution. """

    n_labels = len(labels)

    if n_labels <= 1:
        return 0

    value, counts = np.unique(labels, return_counts=True)
    probs = counts / n_labels
    n_classes = np.count_nonzero(probs)

    if n_classes <= 1:
        return 0

    ent = 0.

    # Compute entropy
    base = e if base is None else base
    for i in probs:
        ent -= i * log(i, base)

    return ent

def calculate_information_gain(match_frequencies: list, prior_entropy: float):
    unique_frequencies = sorted(list({item[1] for item in match_frequencies}))
    split_points = [unique_frequencies[index] + abs((unique_frequencies[index + 1] - unique_frequencies[index]) / 2)
                    for index in range(0, len(unique_frequencies) - 1)]
    best_information_gain = 0
    for split_point in split_points:
        lower_labels = []
        higher_labels = []
        for mf in match_frequencies:  # Split occurances into two groups, based on split point
            if mf[1] < split_point:
                lower_labels.append(mf[0])
            else:
                higher_labels.append(mf[1])

        lower_entropy = calculate_entropy(lower_labels)
        higher_entropy = calculate_entropy(higher_labels)

        total = len(lower_labels) + len(higher_labels)
        lower_prob = len(lower_labels) / total
        higher_prob = len(higher_labels) / total

        information_gain = prior_entropy - (lower_prob * lower_entropy + higher_prob * higher_entropy)
        if information_gain > best_information_gain:
            best_information_gain = information_gain

    return best_information_gain

# --- Shapelet Eval ---

def evaluate_shapelet(class_series: list, window: list):
    prior_entropy = calculate_entropy_set(class_series)
    match_frequencies = []  # [[]]
    for class_index in range(0, len(class_series)):
        for series in class_series[class_index]:
            frequency = match_frequency(series, window)
            match_frequencies.append((class_index, frequency))
    return calculate_information_gain(match_frequencies, prior_entropy)


def generate_best_shaplets(data: list):
    print("---Generating Shapelets---")
    shapelets = []
    windows = generate_windows(data, 2, 20)
    index_pairs = []
    for i in range(0, len(data)):
        for t in range(i + 1, len(data)):
            index_pairs.append((i, t))
    for pair in tqdm(index_pairs):
        #print("Generating optimal shapelet for classes (", pair[0], ",", pair[1], ")")
        best_score = 0
        best_shapelet = []
        for window in windows[pair[0]] + windows[pair[1]]:
            score = evaluate_shapelet([data[i] for i in pair], window)
            if score > best_score:
                best_score = score
                best_shapelet = window
        if best_score != 0:
            shapelets.append(best_shapelet)
         
    print("---Finished Generating Shapelets---")
    return shapelets

data = load_and_format_data(filename + os.sep + "Temp.tsv")
shapelets = generate_best_shaplets(data)

max_length = 0
for shapelet in shapelets:
    if len(shapelet) > max_length:
        max_length = len(shapelet)
    plt.plot(shapelet)
plt.xticks(range(0, max_length, 1))
plt.show()
