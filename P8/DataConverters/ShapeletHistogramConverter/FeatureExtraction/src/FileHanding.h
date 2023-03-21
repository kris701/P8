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
        for (const auto &point : data) {
            out << point;
            if (point != data.back())
                out << "\n";
        }
        out.close();
    }

    static void WriteFile(const std::string &path, const std::vector<std::string> &data) {
        std::filesystem::create_directories(std::filesystem::path(path).parent_path());
        std::ofstream out(path);
        for (const auto &point : data) {
            out << point;
            if (point != data.back())
                out << "\n";
        }
        out.close();
    }
};

#endif //FEATUREEXTRACTION_FILEHANDING_H
