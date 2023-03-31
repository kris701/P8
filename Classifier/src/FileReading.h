#ifndef CLASSIFIER_FILEREADING_H
#define CLASSIFIER_FILEREADING_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <filesystem>
#include "Types.h"

namespace FileReading {
    static Series ReadFeatureSeries(const std::string &path) {
        Series series;

        std::ifstream infile(path);
        double num;
        while (infile >> num)
            series.push_back(num);

        return series;
    }

    static MappedSeries ReadFeatureSeries(const std::vector<std::string> &paths, const std::string &rootPath = "") {
        namespace fs = std::filesystem;
        MappedSeries mappedSeries;
        for (const auto &path : paths) {
            int label = std::atoi(fs::path(fs::path(path).parent_path().string()).filename().string().c_str());
            mappedSeries[label].push_back(ReadFeatureSeries(rootPath + "/" + path));
        }
        return mappedSeries;
    }

    static std::vector<std::string> ReadSplitFile(const std::string &path) {
        std::vector<std::string> paths;

        std::ifstream infile(path);
        std::string tempPath;
        while (infile >> tempPath)
            paths.push_back(tempPath);

        return paths;
    }
}

#endif //CLASSIFIER_FILEREADING_H
