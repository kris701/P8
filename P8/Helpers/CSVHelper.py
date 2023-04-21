import os
import csv

def AppendToCSV(data : list, fileName : str = "result.csv") -> None:
    if not os.path.exists(fileName):
        with open(fileName, 'w', newline='') as csvFile:
            csvWriter = csv.writer(csvFile, delimiter=',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
            csvWriter.writerow(data)
    else:
        with open(fileName, 'a', newline='') as csvFile:
            csvWriter = csv.writer(csvFile, delimiter=',', quotechar='|', quoting=csv.QUOTE_MINIMAL)
            csvWriter.writerow(data)
