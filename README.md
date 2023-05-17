#P8 - Feature Interpretable Feature Histograms (FISH)
This is a project about improving the accuracy of predictions within Few Shot Learning (FSL) of Time Series (TS) data. 
The general idea is to make label classification on a dataset, with only very few datapoints (we used 6 and 8 samples).
This project focused on the feature extraction part of this, by attempting o extract as much data as possible from these few datapoints, and giving them in a representable and interpretable format.
The overall flow of this process can be seen in the following figure:

![overallflow drawio](https://github.com/kris701/P8/assets/22596587/25d100dc-713c-4ab7-981e-9a3fb8d9ef46)

The FISH extractor then, in the end, give you a feature histogram, that represents how well the given datapoints fit within the shapelets:

![FISHintepretability drawio (1)](https://github.com/kris701/P8/assets/22596587/030149c6-17ca-4912-9cbd-bdcf2ab52466)

## Requirements
The feature extractor itself is made in C++, and therefore requires a C++ compiler. It can compile on both Windows and Linux.
Cmake is also required for the build system.
Python is used to bind it all toghether, and to run experiments on.
The current implementation is setup to use the [UCR Archive](https://www.cs.ucr.edu/~eamonn/time_series_data_2018/) as datasets, and all the configs are setup for that archive. You have to place the archive into the `P8/Data` folder.

## How to Use
Assuming you have followed the requirements, the implementation should be able to run out of the box.
All the python stuff is there simply to run experiments on. The file `P8/P8.py` is where the entire implementation runs from, and its also where you can change what configs are run.

However if you only want to run the feature extractor, the C++ executable uses command line arguments to run, so you can run it seperately.
