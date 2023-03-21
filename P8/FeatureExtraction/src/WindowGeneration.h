#ifndef FEATUREEXTRACTION_WINDOWGENERATION_H
#define FEATUREEXTRACTION_WINDOWGENERATION_H

#include "Types.h"
#include "Logger.h"

namespace WindowGeneration {
    [[nodiscard]] static std::vector<Series> GenerateWindows(const Series &series, uint length) {
        std::vector<Series> windows;

        if (series.empty() || length == 0)
            return windows;

        for (uint i = 0; i + length <= series.size(); ++i) {
            Series window;
            const double yOffset = series[i]; // Always start a window at 0
            for (uint offset = 0; offset < length; ++offset)
                window.push_back(series[i + offset] - yOffset);
            windows.push_back(window);
        }

        return windows;
    }

    [[nodiscard]] static std::vector<Series> GenerateWindows(const Series &series, uint minLength, uint maxLength) {
        std::vector<Series> windows;

        for (uint i = minLength; i <= maxLength; i++) {
            auto tWindows = GenerateWindows(series, i);
            for (const auto &window : tWindows)
                windows.push_back(window);
        }

        return windows;
    }

    [[nodiscard]] static std::vector<Series>
    GenerateWindows(const std::vector<LabelledSeries> &series, uint minLength, uint maxLength) {
        uint id = Logger::Begin("Generating windows");
        std::vector<Series> windows;
        uint totalWindows = 0;

        for (const auto &s : series) {
            const auto tempWindows = GenerateWindows(s.series, minLength, maxLength);
            totalWindows += tempWindows.size();
            for (const auto &window : tempWindows)
                windows.push_back(window);
        }
        Logger::End(id);

        /* Duplicate Removal */
        id = Logger::Begin("Removing Duplicate Windows");
        std::sort(windows.begin(), windows.end());
        windows.erase(std::unique(windows.begin(), windows.end()), windows.end());
        Logger::End(id);

        return windows;
    }
}

#endif //FEATUREEXTRACTION_WINDOWGENERATION_H
