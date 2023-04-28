#ifndef FEATUREEXTRACTION_FILEHANDING_H
#define FEATUREEXTRACTION_FILEHANDING_H

#include <string>
#include <optional>
#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <vector>
#include "types/SeriesMap.h"

namespace FileHanding {
    [[nodiscard]] static SeriesMap ReadCSV(const std::string &path, const std::string &delimiter = ",") {
        if (!std::filesystem::exists(path))
            throw std::logic_error("Could not find file " + path);
        std::ifstream file(path);
        std::string line;
        SeriesMap dataPoints;
        int lineNum = 0;
        while (std::getline(file, line)) {
            std::optional<int> type;
            Series series;
            while (!line.empty()) {
                std::string token;
                while (!line.empty() && line.substr(0, delimiter.size()) != delimiter) {
                    token += line[0];
                    line.erase(line.begin());
                }

                if (!type.has_value())
                    type = std::atoi(token.c_str());
                else
                    series.push_back(std::atof(token.c_str()));

                if (!line.empty() && line.substr(0, delimiter.size()) == delimiter)
                    line.erase(line.begin());
            }

            if (type.has_value())
                dataPoints[type.value()].push_back(series);
            else
                throw std::logic_error(&"Missing type on line " [lineNum]);

            lineNum++;
        }
        file.close();
        return dataPoints;
    }

    [[nodiscard]] static SeriesMap ReadCSV(const std::vector<std::string> &paths, const std::string &delimiter = ",") {
        SeriesMap series;

        for (const auto &path : paths) {
            const auto tempSeries = ReadCSV(path, delimiter);
            for (const auto &seriesSet : tempSeries)
                series[seriesSet.first].insert(series[seriesSet.first].end(), tempSeries.at(seriesSet.first).begin(), tempSeries.at(seriesSet.first).end());
        }

        return series;
    }

    static void WriteFile(const std::string &path, const std::vector<double> &data) {
        std::filesystem::create_directories(std::filesystem::path(path).parent_path());
        std::ofstream out(path);
        for (int i = 0; i < data.size(); i++) {
            out << data.at(i);
            if (i != data.size() - 1)
                out << "\n";
        }
        out.close();
    }

    static void WriteFile(const std::string &path, const std::vector<std::string> &data) {
        std::filesystem::create_directories(std::filesystem::path(path).parent_path());
        std::ofstream out(path);
        for (int i = 0; i < data.size(); i++) {
            out << data.at(i);
            if (i != data.size() - 1)
                out << "\n";
        }
        out.close();
    }

    // https://stackoverflow.com/questions/41304891/how-to-count-the-number-of-files-in-a-directory-using-standard
    static std::size_t DirFileCount(std::filesystem::path path) {
        using std::filesystem::directory_iterator;
        return std::distance(directory_iterator(path), directory_iterator{});
    }

    static std::vector<std::string> WriteToFiles(const std::string &dir, const std::vector<std::vector<double>> &data) {
        std::vector<std::string> paths;

        uint startIndex = 0;
        if (std::filesystem::exists(dir))
            startIndex = DirFileCount(dir);

        for (uint i = 0; i < data.size(); i++) {
            const std::string path = dir + std::to_string(startIndex + i);
            WriteFile(path, data.at(i));
            paths.push_back(path);
        }

        return paths;
    }

    static std::vector<std::string> WriteToFiles(const std::string &dir, const std::unordered_map<uint, std::vector<std::vector<double>>> &data) {
        std::vector<std::string> paths;

        for (const auto &classSet : data) {
            const std::string classDir = dir + std::to_string(classSet.first) + "/";
            for (const auto &path : WriteToFiles(classDir, classSet.second))
                paths.push_back(path);
        }

        return paths;
    }

    static std::vector<std::string> RemoveSubPath(const std::string &subPath, const std::vector<std::string> &paths) {
        std::vector<std::string> newPaths;
        const uint subPathLength = subPath.size();
        for (auto &p : paths) {
            auto subPathIndex = p.find(subPath);
            newPaths.push_back(p.substr(subPathIndex + subPathLength, p.size()));
        }
        return newPaths;
    }

    static void WriteCSV
    (const std::string &path, const std::optional<std::vector<std::string>> &header, const std::vector<std::vector<std::string>> &lines) {
        std::filesystem::create_directories(std::filesystem::path(path).parent_path());
        std::ofstream out(path);
        if (header.has_value()) {
            for (const auto &p: header.value()) {
                out << p;
                if (&p != &header.value().back())
                    out << ',';
            }
            if (!lines.empty())
                out << '\n';
        }
        for (const auto &l : lines) {
            for (const auto &p: l) {
                out << p;
                if (&p != &l.back())
                    out << ',';
            }
            if (&l != &lines.back())
                out << '\n';
        }
        out.close();
    }
};

#endif //FEATUREEXTRACTION_FILEHANDING_H
