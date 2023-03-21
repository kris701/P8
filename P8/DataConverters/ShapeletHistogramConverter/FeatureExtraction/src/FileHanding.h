#ifndef FEATUREEXTRACTION_FILEHANDING_H
#define FEATUREEXTRACTION_FILEHANDING_H

#include <string>
#include <optional>
#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <vector>
#include "Types.h"

namespace FileHanding {
    [[nodiscard]] static std::vector<LabelledSeries> ReadCSV(const std::string &path, const std::string &delimiter = ",") {
        if (!std::filesystem::exists(path))
            throw std::logic_error("Could not find file " + path);
        std::ifstream file(path);
        std::string line;
        std::vector<LabelledSeries> dataPoints;
        int lineNum = 0;
        while (std::getline(file, line)) {
            std::optional<int> type;
            std::vector<double> series;
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
                dataPoints.emplace_back(type.value(), series);
            else
                throw std::logic_error(&"Missing type on line " [lineNum]);

            lineNum++;
        }
        file.close();
        return dataPoints;
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
    std::size_t DirFileCount(std::filesystem::path path) {
        using std::filesystem::directory_iterator;
        return std::distance(directory_iterator(path), directory_iterator{});
    }

    static std::vector<std::string> WriteToFiles(const std::string &dir, const std::unordered_map<int, std::vector<std::vector<double>>> &data) {
        std::vector<std::string> paths;

        for (const auto &classSet : data) {
            const std::string classDir = dir + std::to_string(classSet.first) + "/";
            uint startIndex = 0;
            if (std::filesystem::exists(classDir))
                startIndex = DirFileCount(classDir);

            for (uint i = 0; i < classSet.second.size(); i++) {
                const std::string path = classDir + std::to_string(startIndex + i);
                WriteFile(path, classSet.second.at(i));
                paths.push_back(path);
            }
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
};

#endif //FEATUREEXTRACTION_FILEHANDING_H
