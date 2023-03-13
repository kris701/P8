import os
from math import log, e
import matplotlib.pyplot as plt

import numpy as np
import pandas as pd
from tqdm import tqdm

dirname = os.path.dirname(__file__)
filename = os.path.join(dirname, "data", "UCRArchive_2018", "SwedishLeaf")

# --- Data Loading ---

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


def generate_windows(series: list, min: int, max: int):
    print("---Generating Windows---")
    print("Total Series: ", len(series))
    windows = []
    for s in series:
        for length in range(min, max + 1):
            for w in generate_windows2(s, length):
                if w not in windows:
                    windows.append(w)
    print("Total Windows: ", len(windows))
    print("---Finished Generating Windows---")
    return windows

# --- Match Frequency ---

def is_match(s1: list, s2: list):
    offset = s1[0] - s2[0]

    tolerance = 0.01
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

def evaluate_shapelet(series: list, labels: list, window: list):
    prior_entropy = calculate_entropy(labels)
    match_frequencies = []  # (label, frequency)
    for label_series in zip(series, labels):
        frequency = match_frequency(label_series[0], window)
        match_frequencies.append((label_series[1], frequency))
    return calculate_information_gain(match_frequencies, prior_entropy)


def generate_best_shaplets(series: list, labels: list, count: int):
    print("---Generating Shapelets---")
    shapelets = []
    windows = generate_windows(series, 2, 10)
    print("Evaluating each possible shapelet")
    for window in tqdm(windows):
        score = evaluate_shapelet(series, labels, window)
        if len(shapelets) < count:
            shapelets.append((score, window))
            shapelets = sorted(shapelets)
        else:
            for i in range(0, len(shapelets)):
                if score > shapelets[i][0]:
                    shapelets.insert(i, (score, window))
                    shapelets.pop()
                    break
    print("---Finished Generating Shapelets---")
    return shapelets


data_series, data_labels = load_data(filename + os.sep + "Temp.tsv")

shapelets = generate_best_shaplets(data_series, data_labels, 10)
max_length = 0
for shapelet in shapelets:
    if len(shapelet[1]) > max_length:
        max_length = len(shapelet[1])
    plt.plot(shapelet[1])
plt.xticks(range(0, max_length, 1))
plt.show()
