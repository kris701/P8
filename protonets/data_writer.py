import pandas as pd
FP = "path to swedish leaf data set"

def load_and_format_data(fp):
    df = pd.read_csv(fp, delimiter="\t", header=None)
    X = df.iloc[:, 1:]
    y = df.iloc[:, 0]
    classes = []
    for c in range(len(set(y))):
        classes.append([X.iloc[i] for i in range(len(y)) if y[i] == c])
    return classes


