# Formatted datasets
This folder contains the formated datasets, that was outputted from the `DataConverter`s.
The structure is as follows, there will be a folder for each dataset conversion, that contains:
* `data`
* `features` (Only for `ShapeletHistogramConverter`)
* `split`

## Data 
Contains the "raw" TS data.
It is organized such that each folder is a class, where each folder then contains `n` amount of samples

## Features (Only for `ShapeletHistogramConverter`)
Contains information on what shapelets the histogram is made out of, as well as what attributes was used.

## Split
Contains text files that describe what is the `train` set, `val` set and `test` set.
