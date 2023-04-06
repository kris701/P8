#ifndef FEATUREEXTRACTION_WINDOWGENERATION_H
#define FEATUREEXTRACTION_WINDOWGENERATION_H

#include "misc/Constants.h"
#include "IO/Logger.h"

namespace WindowGeneration {
    /// <summary>
    /// Generates a vector of sub-series out of a series, based on a specific window size.
    /// </summary>
    /// <param name="series">A vector of doubles</param>
    /// <param name="length">An integer, representing the length of each window to be made</param>
    /// <returns>A vector of sub-series</returns>
    [[nodiscard]] static std::vector<Series> GenerateWindowsOfLength(const Series &series, uint length) {
        std::vector<Series> windows;

        if (series.empty() || length == 0)
            return windows;

        for (uint i = 0; i < series.size(); i++) {
            if (i + length <= series.size()) {
                Series window;
                for (uint j = 0; j < length; j++)
                    window.push_back(series[i + j]);
                windows.push_back(window);
            }
        }

        return windows;
    }

    /// <summary>
    /// Generates a vector of sub-series out of a series, based on all possible window sized between a min and max length
    /// </summary>
    /// <param name="series">A vector of doubles</param>
    /// <param name="minLength">An inclusive minimum length of a window</param>
    /// <param name="maxLength">An inclusive maximum length of a window</param>
    /// <returns>A vector of sub-series</returns>
    [[nodiscard]] static std::vector<Series> GenerateWindowsOfMinMaxLength(const Series &series, uint minLength, uint maxLength) {
        std::vector<Series> windows;
        if (series.size() == 0)
            return windows;
        if (maxLength == 0 || maxLength > series.size())
            maxLength = series.size();
        if (minLength > maxLength)
            return windows;

        for (uint i = minLength; i <= maxLength; i++) {
            auto tWindows = GenerateWindowsOfLength(series, i);
            for (const auto &window : tWindows)
                windows.push_back(window);
        }

        return windows;
    }

    /// <summary>
    /// Removes duplicate series from a vector of series
    /// </summary>
    /// <param name="windows">A vector of series</param>
    [[nodiscard]] static void RemoveDuplicateWindows(std::vector<Series> *windows) {
        std::sort(windows->begin(), windows->end());
        windows->erase(std::unique(windows->begin(), windows->end()), windows->end());
    }

    /// <summary>
    /// Generates a vector of series, based on a a vector of LabelledSeries
    /// </summary>
    /// <param name="series">A vector of series</param>
    /// <param name="minLength">An inclusive minimum length of a window</param>
    /// <param name="maxLength">An inclusive maximum length of a window</param>
    /// <returns>A vector of windows</returns>
    [[nodiscard]] static std::vector<Series> GenerateWindows(const std::vector<LabelledSeries> &series, uint minLength, uint maxLength) {
        std::vector<Series> windows;

        for (const auto &s : series) {
            const auto tempWindows = GenerateWindowsOfMinMaxLength(s.series, minLength, maxLength);
            for (const auto &window : tempWindows)
                windows.push_back(window);
        }

        RemoveDuplicateWindows(&windows);

        return windows;
    }
}

#endif //FEATUREEXTRACTION_WINDOWGENERATION_H
